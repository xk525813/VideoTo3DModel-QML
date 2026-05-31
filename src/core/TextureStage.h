#pragma once

#include "IStage.h"

class TextureStage : public IStage {
public:
    QString name() const override { return "texture"; }

    StageResult execute(const QString& projectDir,
                        const QJsonObject& config, const ProgressCallback& onProgress = {}) override;
};
