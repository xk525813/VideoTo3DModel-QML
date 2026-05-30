// /Share/GenerateModel/src/core/FrameExtractor.h
#pragma once

#include "IStage.h"
#include <QString>
#include <QJsonArray>

class FrameExtractor : public IStage {
public:
    QString name() const override { return "frame_extraction"; }

    StageResult execute(const QString& projectDir,
                        const QJsonObject& config) override;

    /// 用 OpenCV 对帧做模糊检测，返回质量评分 0.0-1.0
    static double blurScore(const QString& imagePath);
};
