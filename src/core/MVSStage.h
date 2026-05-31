// /Share/GenerateModel/src/core/MVSStage.h
#pragma once

#include "IStage.h"

class MVSStage : public IStage {
public:
    QString name() const override { return "mvs"; }

    StageResult execute(const QString& projectDir,
                        const QJsonObject& config, const ProgressCallback& onProgress = {}) override;

private:
    /// 将 COLMAP 输出转换为 OpenMVS 输入格式
    StageResult convertCOLMAPtoMVS(const QString& projectDir,
                                    const QJsonObject& tools,
                                    const ProgressCallback& onProgress);

    // removed: use onProgress callback(double progress, const QString& status, int etaSeconds);
};
