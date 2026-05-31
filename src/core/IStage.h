// /Share/GenerateModel/src/core/IStage.h
#pragma once

#include <QString>
#include <QJsonObject>
#include <functional>

struct StageResult {
    bool ok = true;
    QString errorMessage;

    static StageResult success() { return {true, {}}; }
    static StageResult failure(const QString& msg) { return {false, msg}; }
};

/// 进度回调: (progress 0.0~1.0, statusText, etaSeconds)
using ProgressCallback = std::function<void(double, const QString&, int)>;

class IStage {
public:
    virtual ~IStage() = default;

    /// 返回该阶段的唯一标识名
    virtual QString name() const = 0;

    /// 执行此阶段
    /// @param projectDir  项目根目录的绝对路径
    /// @param config      项目配置 (来自 config/settings.json)
    /// @param onProgress  进度回调，可选
    /// @return 执行结果
    virtual StageResult execute(const QString& projectDir,
                                const QJsonObject& config,
                                const ProgressCallback& onProgress = {}) = 0;
};
