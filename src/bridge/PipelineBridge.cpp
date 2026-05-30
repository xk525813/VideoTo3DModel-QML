// /Share/GenerateModel/src/bridge/PipelineBridge.cpp
#include "PipelineBridge.h"
#include "../core/FrameExtractor.h"
#include "../core/SfMStage.h"
#include "../core/MVSStage.h"
#include "../core/TextureStage.h"
#include "../core/ExportManager.h"
#include "../core/ProjectManager.h"
#include "../core/LogManager.h"
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

QVariantMap PipelineBridge::checkDependencies() {
    QVariantMap deps;
    auto check = [](const QString& cmd, const QStringList& args = {}) -> bool {
        QProcess p;
        p.start(cmd, args);
        p.waitForFinished(5000);
        return p.exitCode() == 0 || p.error() == QProcess::Timedout;
    };

    deps["ffmpeg"] = check("ffmpeg", {"-version"});
    deps["colmap"] = check("colmap", {"--help"});
    deps["DensifyPointCloud"] = check("DensifyPointCloud", {"--help"});
    deps["ReconstructMesh"] = check("ReconstructMesh", {"--help"});
    deps["InterfaceCOLMAP"] = check("InterfaceCOLMAP", {"--help"});
    deps["obj2gltf"] = check("obj2gltf", {"--help"});

    return deps;
}

void PipelineBridge::startPipeline(const QString& videoPath,
                                    const QVariantMap& settings) {
    // 创建项目目录
    QString baseDir = settings.value("outputDir", QDir::homePath() + "/VideoTo3D_Projects").toString();
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

QString PipelineBridge::checkUnfinishedProject(const QString& projectDir) {
    QString last = ProjectManager::lastCompletedStage(projectDir);
    if (last.isEmpty() || last == "export") {
        return {}; // 没有未完成的或已完成
    }
    return projectDir;
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
    LogManager::debug("阶段完成信号: {}", name.toStdString());
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
