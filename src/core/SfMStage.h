// /Share/GenerateModel/src/core/SfMStage.h
#pragma once

#include "IStage.h"

class SfMStage : public IStage {
public:
    QString name() const override { return "sfm"; }

    StageResult execute(const QString& projectDir,
                        const QJsonObject& config, const ProgressCallback& onProgress = {}) override;

private:
    /// 向 stdout 输出进度行 (由 QProcess 捕获 → PipelineBridge)
    // removed: use onProgress callback(double progress, const QString& status, int etaSeconds);
};
