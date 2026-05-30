// /Share/GenerateModel/src/core/PipelineOrchestrator.h
#pragma once

#include "IStage.h"
#include "ProjectManager.h"
#include <QObject>
#include <QList>
#include <QJsonObject>

/// 管线运行状态
enum class PipelineState {
    Idle,
    Running,
    Paused,
    Completed,
    Failed,
    Cancelled
};

class PipelineOrchestrator : public QObject {
    Q_OBJECT

public:
    explicit PipelineOrchestrator(QObject* parent = nullptr);
    ~PipelineOrchestrator();

    /// 注册管线阶段 (所有权转移给 orchestrator)
    void addStage(IStage* stage);

    /// 运行完整管线 (在 worker 线程中调用，阻塞直到完成或失败)
    void run(const QString& projectDir, const QJsonObject& config);

    /// 从上次中断处继续
    void resume(const QString& projectDir, const QJsonObject& config);

    /// 取消运行 (线程安全)
    void cancel();

    PipelineState state() const { return m_state; }

signals:
    void stageStarted(const QString& stageName);
    void stageCompleted(const QString& stageName);
    void stageFailed(const QString& stageName, const QString& errorMessage);
    void progressUpdated(const QString& stageName, double progress,
                         const QString& status, int etaSeconds);
    void pipelineFinished(bool success, const QString& message);
    void stateChanged(PipelineState state);

private:
    QList<IStage*> m_stages;
    PipelineState m_state = PipelineState::Idle;
    bool m_cancelled = false;

    void setState(PipelineState s);
};
