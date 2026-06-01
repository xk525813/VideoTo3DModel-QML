// /Share/GenerateModel/src/bridge/PipelineBridge.cpp
#include "PipelineBridge.h"
#include "../core/FrameExtractor.h"
#include "../core/SfMStage.h"
#include "../core/MVSStage.h"
#include "../core/TextureStage.h"
#include "../core/ExportManager.h"
#include "../core/ProjectManager.h"
#include "../core/LogManager.h"
#include <QCoreApplication>
#include <QProcess>
#include <QDir>
#include <QFileInfo>
#include <QDateTime>

// ─── PipelineWorker ──────────────────────────────────────────────

PipelineWorker::PipelineWorker(QObject* parent) : QObject(parent) {
    // 在 worker 构造时注册所有管线阶段 (所有权通过 QObject parent 管理)
    m_orchestrator.addStage(new FrameExtractor());
    m_orchestrator.addStage(new SfMStage());
    m_orchestrator.addStage(new MVSStage());
    m_orchestrator.addStage(new TextureStage());
    m_orchestrator.addStage(new ExportManager());

    // 转发 orchestrator 的信号
    connect(&m_orchestrator, &PipelineOrchestrator::stageStarted,
            this, &PipelineWorker::stageStarted);
    connect(&m_orchestrator, &PipelineOrchestrator::stageCompleted,
            this, &PipelineWorker::stageCompleted);
    connect(&m_orchestrator, &PipelineOrchestrator::stageFailed,
            this, &PipelineWorker::stageFailed);
    connect(&m_orchestrator, &PipelineOrchestrator::progressUpdated,
            this, &PipelineWorker::progressUpdated);
    connect(&m_orchestrator, &PipelineOrchestrator::pipelineFinished,
            this, &PipelineWorker::finished);
}

void PipelineWorker::setProject(const QString& projectDir,
                                 const QJsonObject& config) {
    m_projectDir = projectDir;
    m_config = config;
}

void PipelineWorker::process() {
    m_orchestrator.run(m_projectDir, m_config);
}

void PipelineWorker::cancel() {
    m_orchestrator.cancel();
}

// ─── PipelineBridge ──────────────────────────────────────────────

PipelineBridge::PipelineBridge(QObject* parent) : QObject(parent) {
    detectHardware();
    setPipelineState("idle");
}

void PipelineBridge::detectHardware() {
    auto profile = HardwareDetector::detect();
    m_hwProfile = profile.toJson().toVariantMap();
    emit hardwareProfileChanged();
}

QString PipelineBridge::resolveTool(const QString& name)
{
    QString appDir = QCoreApplication::applicationDirPath();

    // 部署模式: 工具在可执行文件同级的 tools/ 下
    QString bundled = appDir + "/tools/" + name;
    if (QFile::exists(bundled))
        return bundled;

    // 开发模式: 工具在项目根目录 tools/ 下 (exe 在 build/src/)
    bundled = appDir + "/../../../tools/" + name;
    if (QFile::exists(bundled))
        return bundled;

    return name; // 回退到系统 PATH
}

QVariantMap PipelineBridge::checkDependencies() {
    QVariantMap deps;
    auto check = [](const QString& cmd, const QStringList& args = {}) -> QVariantMap {
        QVariantMap info;
        QProcess p;
        p.start(cmd, args);
        p.waitForFinished(5000);
        info["available"] = p.exitCode() == 0 || p.error() == QProcess::Timedout;
        info["path"] = cmd;
        // 标记来源：内建(bundled) vs 系统(system)
        info["bundled"] = cmd.contains("/tools/");
        return info;
    };

    deps["ffmpeg"]              = check(resolveTool("ffmpeg"),              {"-version"});
    deps["colmap"]              = check(resolveTool("colmap"),              {"--help"});
    deps["DensifyPointCloud"]   = check(resolveTool("DensifyPointCloud"),   {"--help"});
    deps["ReconstructMesh"]     = check(resolveTool("ReconstructMesh"),     {"--help"});
    deps["InterfaceCOLMAP"]     = check(resolveTool("InterfaceCOLMAP"),     {"--help"});
    deps["RefineMesh"]          = check(resolveTool("RefineMesh"),          {"--help"});
    deps["obj2gltf"]            = check(resolveTool("obj2gltf"),            {"--help"});

    return deps;
}

void PipelineBridge::startPipeline(const QString& videoPath,
                                    const QVariantMap& settings) {
    // 创建项目目录：用户指定目录优先，否则在运行目录的 projects/ 下
    QString userDir = settings.value("outputDir").toString();
    QString baseDir = userDir.isEmpty()
        ? QDir::currentPath() + "/projects"
        : userDir;
    QString projectName = "Project_" + QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss");
    QString projectDir = ProjectManager::createProject(baseDir, projectName);

    // 复制视频到项目
    QString destVideo = projectDir + "/source/" + QFileInfo(videoPath).fileName();
    QFile::copy(videoPath, destVideo);

    // 加载硬件配置和策略
    auto profile = HardwareDetector::detect();
    bool userGpu = settings.value("gpuEnabled", true).toBool();
    QString quality = settings.value("quality", "standard").toString();
    auto strategy = HardwareDetector::determineStrategy(profile, userGpu, quality);

    QJsonObject config;
    config["pipelineStrategy"] = QJsonObject{
        {"quality", strategy.quality},
        {"useGPU", strategy.useGPU},
        {"maxFrameDimension", strategy.maxFrameDimension},
        {"denseQuality", strategy.denseQuality}
    };
    config["exportFormat"] = settings.value("exportFormat", "glb").toString();
    config["projectDir"] = projectDir;

    // 解析工具路径（优先 tools/ 内建）
    QJsonObject toolPaths;
    toolPaths["ffmpeg"]            = resolveTool("ffmpeg");
    toolPaths["colmap"]            = resolveTool("colmap");
    toolPaths["DensifyPointCloud"] = resolveTool("DensifyPointCloud");
    toolPaths["ReconstructMesh"]   = resolveTool("ReconstructMesh");
    toolPaths["InterfaceCOLMAP"]   = resolveTool("InterfaceCOLMAP");
    toolPaths["RefineMesh"]        = resolveTool("RefineMesh");
    toolPaths["obj2gltf"]          = resolveTool("obj2gltf");
    config["tools"] = toolPaths;

    // 创建 worker (stages 在 worker 构造时自注册到 orchestrator)
    auto* worker = new PipelineWorker();
    worker->setProject(projectDir, config);

    m_worker = worker;
    m_worker->moveToThread(&m_workerThread);

    connect(&m_workerThread, &QThread::finished, m_worker, &QObject::deleteLater);

    // worker signals → bridge slots
    connect(m_worker, &PipelineWorker::stageStarted,
            this, &PipelineBridge::onStageStarted);
    connect(m_worker, &PipelineWorker::stageCompleted,
            this, &PipelineBridge::onStageCompleted);
    connect(m_worker, &PipelineWorker::stageFailed,
            this, &PipelineBridge::onStageFailed);
    connect(m_worker, &PipelineWorker::progressUpdated,
            this, &PipelineBridge::onProgressUpdated);
    connect(m_worker, &PipelineWorker::finished,
            this, &PipelineBridge::onPipelineFinished);

    setPipelineState("running");
    m_workerThread.start();
    QMetaObject::invokeMethod(m_worker, "process", Qt::QueuedConnection);
}

void PipelineBridge::cancelPipeline() {
    if (m_worker) {
        QMetaObject::invokeMethod(m_worker, "cancel", Qt::QueuedConnection);
    }
}

QString PipelineBridge::scanForUnfinishedProject() {
    QString projectsRoot = QDir::currentPath() + "/projects";
    QDir dir(projectsRoot);
    if (!dir.exists()) return {};

    QStringList entries = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Time);
    for (const auto& entry : entries) {
        QString projectDir = projectsRoot + "/" + entry;
        QString last = ProjectManager::lastCompletedStage(projectDir);
        // 有未完成的阶段（非空且未到 export）
        if (!last.isEmpty() && last != "export") {
            return projectDir;
        }
    }
    return {};
}

void PipelineBridge::resumePipeline(const QString& projectDir) {
    QJsonObject settings = ProjectManager::loadSettings(projectDir);
    if (settings.isEmpty()) {
        emit pipelineError("恢复失败", "无法加载项目配置文件");
        return;
    }

    // 复用 startPipeline 的 worker 创建逻辑
    auto* worker = new PipelineWorker();
    worker->setProject(projectDir, settings);

    m_worker = worker;
    m_worker->moveToThread(&m_workerThread);

    connect(&m_workerThread, &QThread::finished, m_worker, &QObject::deleteLater);
    connect(m_worker, &PipelineWorker::stageStarted,
            this, &PipelineBridge::onStageStarted);
    connect(m_worker, &PipelineWorker::stageCompleted,
            this, &PipelineBridge::onStageCompleted);
    connect(m_worker, &PipelineWorker::stageFailed,
            this, &PipelineBridge::onStageFailed);
    connect(m_worker, &PipelineWorker::progressUpdated,
            this, &PipelineBridge::onProgressUpdated);
    connect(m_worker, &PipelineWorker::finished,
            this, &PipelineBridge::onPipelineFinished);

    setPipelineState("running");
    m_workerThread.start();
    QMetaObject::invokeMethod(m_worker, "process", Qt::QueuedConnection);
}

void PipelineBridge::clearAndRestart(const QString& projectDir) {
    ProjectManager::clearIntermediateFiles(projectDir);
    ProjectManager::resetCheckpoints(projectDir);
}

// ─── Slot implementations ───────────────────────────────────────

void PipelineBridge::onStageStarted(const QString& name) {
    m_currentStage = name;
    m_stageProgress = 0;
    m_stageStatus = "";
    m_stageEta = 0;
    emit currentStageChanged();
    emit stageProgressChanged();
    emit stageStatusChanged();
    emit stageEtaChanged();
}

void PipelineBridge::onStageCompleted(const QString& name) {
    LogManager::debug(spdlog::source_loc{__FILE__, __LINE__, __FUNCTION__},
                      "阶段完成: {}", name.toStdString());
}

void PipelineBridge::onStageFailed(const QString& name, const QString& error) {
    setPipelineState("failed");
    emit pipelineError(
        QStringLiteral("管线执行失败"),
        error);
}

void PipelineBridge::onProgressUpdated(const QString& stage, double prog,
                                        const QString& status, int eta) {
    if (m_currentStage != stage) {
        m_currentStage = stage;
        emit currentStageChanged();
    }
    m_stageProgress = prog;
    m_stageStatus = status;
    m_stageEta = eta;
    emit stageProgressChanged();
    emit stageStatusChanged();
    emit stageEtaChanged();
}

void PipelineBridge::onPipelineFinished(bool success, const QString& message) {
    m_workerThread.quit();
    m_workerThread.wait();
    setPipelineState(success ? "completed" : "failed");
    emit pipelineFinished(success, message);
}

void PipelineBridge::setPipelineState(const QString& s) {
    if (m_pipelineState != s) {
        m_pipelineState = s;
        emit pipelineStateChanged();
    }
}
