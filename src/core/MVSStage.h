// /Share/GenerateModel/src/core/MVSStage.h
#pragma once

#include "IStage.h"

class MVSStage : public IStage {
public:
    QString name() const override { return "mvs"; }

    StageResult execute(const QString& projectDir,
                        const QJsonObject& config) override;

private:
    /// 将 COLMAP 输出转换为 OpenMVS 输入格式
    StageResult convertCOLMAPtoMVS(const QString& projectDir);

    static void reportProgress(double progress, const QString& status, int etaSeconds);
};
