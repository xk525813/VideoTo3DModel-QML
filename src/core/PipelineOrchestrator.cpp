// /Share/GenerateModel/src/core/PipelineOrchestrator.cpp
#include "PipelineOrchestrator.h"
#include <QDebug>
#include <QThread>

PipelineOrchestrator::PipelineOrchestrator(QObject* parent)
    : QObject(parent) {}

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
            qDebug() << "Skipping completed stage:" << stageName;
            continue;
        }

        emit stageStarted(stageName);

        StageResult result = stage->execute(projectDir, config);

        if (!result.ok) {
            emit stageFailed(stageName, result.errorMessage);
            setState(PipelineState::Failed);
            emit pipelineFinished(false, result.errorMessage);
            return;
        }

        // 标记完成
        ProjectManager::markStageComplete(projectDir, stageName, true);
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
