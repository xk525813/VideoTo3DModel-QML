// /Share/GenerateModel/src/core/PipelineOrchestrator.cpp
#include "PipelineOrchestrator.h"
#include "LogManager.h"
#include <QThread>

PipelineOrchestrator::PipelineOrchestrator(QObject* parent)
    : QObject(parent) {}

PipelineOrchestrator::~PipelineOrchestrator() {
    qDeleteAll(m_stages);
    m_stages.clear();
}

void PipelineOrchestrator::addStage(IStage* stage) {
    m_stages.append(stage);
}

void PipelineOrchestrator::setState(PipelineState s) {
    if (m_state != s) {
        m_state = s;
        emit stateChanged(s);
    }
}

void PipelineOrchestrator::run(const QString& projectDir,
                                const QJsonObject& config) {
    m_cancelled = false;
    setState(PipelineState::Running);

    for (auto* stage : m_stages) {
        if (m_cancelled) {
            setState(PipelineState::Cancelled);
            emit pipelineFinished(false, QStringLiteral("管线已取消"));
            return;
        }

        QString stageName = stage->name();

        // 断点续传: 跳过已完成的阶段
        if (ProjectManager::isStageComplete(projectDir, stageName)) {
            LogManager::debug(spdlog::source_loc{__FILE__, __LINE__, __FUNCTION__},
                              "跳过已完成阶段: {}", stageName.toStdString());
            continue;
        }

        LogManager::info(spdlog::source_loc{__FILE__, __LINE__, __FUNCTION__},
                         "▶ 阶段开始: {}", stageName.toStdString());
        emit stageStarted(stageName);
        emit progressUpdated(stageName, 0.0, "开始...", 0);

        // 构造进度回调，将 stage 内部进度转发到 QML
        auto onProgress = [this, &stageName](double p, const QString& status, int eta) {
            emit progressUpdated(stageName, p, status, eta);
        };

        StageResult result = stage->execute(projectDir, config, onProgress);

        if (!result.ok) {
            LogManager::error(spdlog::source_loc{__FILE__, __LINE__, __FUNCTION__},
                              "✘ 阶段失败: {} — {}",
                              stageName.toStdString(),
                              result.errorMessage.toStdString());
            emit stageFailed(stageName, result.errorMessage);
            setState(PipelineState::Failed);
            //emit pipelineFinished(false, result.errorMessage);
            return;
        }

        // 标记完成
        ProjectManager::markStageComplete(projectDir, stageName, true);
        LogManager::info(spdlog::source_loc{__FILE__, __LINE__, __FUNCTION__},
                         "✔ 阶段完成: {}", stageName.toStdString());
        emit stageCompleted(stageName);
    }

    setState(PipelineState::Completed);
    emit pipelineFinished(true, QStringLiteral("3D 模型生成完成！"));
}

void PipelineOrchestrator::resume(const QString& projectDir,
                                   const QJsonObject& config) {
    run(projectDir, config); // run 默认跳过已完成阶段
}

void PipelineOrchestrator::cancel() {
    m_cancelled = true;
}
