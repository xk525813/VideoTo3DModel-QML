// /Share/GenerateModel/src/bridge/PipelineBridge.h
#pragma once

#include <QObject>
#include <QThread>
#include <QJsonObject>
#include <QVariantMap>
#include "../core/HardwareDetector.h"
#include "../core/PipelineOrchestrator.h"

class PipelineWorker : public QObject {
    Q_OBJECT
public:
    explicit PipelineWorker(QObject* parent = nullptr);
    void setProject(const QString& projectDir, const QJsonObject& config);

public slots:
    void process();
    void cancel();

signals:
    void stageStarted(const QString& stageName);
    void stageCompleted(const QString& stageName);
    void stageFailed(const QString& stageName, const QString& error);
    void progressUpdated(const QString& stage, double p, const QString& s, int eta);
    void finished(bool success, const QString& message);

private:
    PipelineOrchestrator m_orchestrator;
    QString m_projectDir;
    QJsonObject m_config;
};

class PipelineBridge : public QObject {
    Q_OBJECT
    Q_PROPERTY(QVariantMap hardwareProfile READ hardwareProfile NOTIFY hardwareProfileChanged)
    Q_PROPERTY(QString pipelineState READ pipelineState NOTIFY pipelineStateChanged)
    Q_PROPERTY(QString currentStage READ currentStage NOTIFY currentStageChanged)
    Q_PROPERTY(double stageProgress READ stageProgress NOTIFY stageProgressChanged)
    Q_PROPERTY(QString stageStatus READ stageStatus NOTIFY stageStatusChanged)
    Q_PROPERTY(int stageEta READ stageEta NOTIFY stageEtaChanged)

public:
    explicit PipelineBridge(QObject* parent = nullptr);

    QVariantMap hardwareProfile() const { return m_hwProfile; }
    QString pipelineState() const { return m_pipelineState; }
    QString currentStage() const { return m_currentStage; }
    double stageProgress() const { return m_stageProgress; }
    QString stageStatus() const { return m_stageStatus; }
    int stageEta() const { return m_stageEta; }

    /// 解析工具的路径：优先 tools/ 内建，再回退系统 PATH
    Q_INVOKABLE static QString resolveTool(const QString& name);

    /// 检查 COLMAP/OpenMVS/FFmpeg 等必需工具是否可用
    Q_INVOKABLE QVariantMap checkDependencies();

    /// 开始运行管线
    Q_INVOKABLE void startPipeline(const QString& videoPath, const QVariantMap& settings);

    /// 取消运行
    Q_INVOKABLE void cancelPipeline();

    /// 扫描默认项目目录，查找未完成的项目
    Q_INVOKABLE QString scanForUnfinishedProject();

    /// 恢复未完成的项目（从 checkpoint 继续）
    Q_INVOKABLE void resumePipeline(const QString& projectDir);

    /// 清除中间文件，保留源视频和配置，重新开始
    Q_INVOKABLE void clearAndRestart(const QString& projectDir);

signals:
    void hardwareProfileChanged();
    void pipelineStateChanged();
    void currentStageChanged();
    void stageProgressChanged();
    void stageStatusChanged();
    void stageEtaChanged();
    void pipelineError(const QString& title, const QString& message);
    void pipelineFinished(bool success, const QString& message);

private slots:
    void onStageStarted(const QString& name);
    void onStageCompleted(const QString& name);
    void onStageFailed(const QString& name, const QString& error);
    void onProgressUpdated(const QString& stage, double prog, const QString& status, int eta);
    void onPipelineFinished(bool success, const QString& message);

private:
    QVariantMap m_hwProfile;
    QString m_pipelineState;
    QString m_currentStage;
    double m_stageProgress = 0;
    QString m_stageStatus;
    int m_stageEta = 0;

    QThread m_workerThread;
    PipelineWorker* m_worker = nullptr;

    void setPipelineState(const QString& s);
    void detectHardware();
};
