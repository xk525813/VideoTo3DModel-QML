// /Share/GenerateModel/src/core/SfMStage.cpp
#include "SfMStage.h"
#include "ProjectManager.h"
#include <QProcess>
#include <QDir>
#include <QDebug>
#include <QJsonObject>
#include <QJsonDocument>
#include <QFile>
#include <iostream>

void SfMStage::reportProgress(double progress, const QString& status, int etaSeconds) {
    QJsonObject msg;
    msg["stage"] = "sfm";
    msg["progress"] = progress;
    msg["status"] = status;
    msg["eta_seconds"] = etaSeconds;
    std::cout << QJsonDocument(msg).toJson(QJsonDocument::Compact).toStdString()
              << std::endl;
}

StageResult SfMStage::execute(const QString& projectDir,
                               const QJsonObject& config) {
    QString framesDir = projectDir + "/frames";
    QString sfmDir = projectDir + "/sfm";
    QString dbPath = sfmDir + "/database.db";

    bool useGPU = config["pipelineStrategy"].toObject()["useGPU"].toBool(true);

    // Step 1: COLMAP feature_extractor
    reportProgress(0.1, "extracting_features", 0);

    QProcess extractor;
    extractor.start("colmap", {
        "feature_extractor",
        "--database_path", dbPath,
        "--image_path", framesDir,
        "--SiftExtraction.use_gpu", useGPU ? "1" : "0",
        "--SiftExtraction.max_image_size", "2000"
    });
    extractor.waitForFinished(600000); // 10 分钟超时

    if (extractor.exitCode() != 0) {
        return StageResult::failure(
            QStringLiteral("COLMAP 特征提取失败。请确认视频中有足够纹理特征。\n详情: %1")
            .arg(QString(extractor.readAllStandardError())));
    }

    // Step 2: COLMAP exhaustive_matcher
    reportProgress(0.4, "matching_features", 0);

    QProcess matcher;
    matcher.start("colmap", {
        "exhaustive_matcher",
        "--database_path", dbPath,
        "--SiftMatching.use_gpu", useGPU ? "1" : "0"
    });
    matcher.waitForFinished(600000);

    if (matcher.exitCode() != 0) {
        return StageResult::failure(
            QStringLiteral("COLMAP 特征匹配失败。\n详情: %1")
            .arg(QString(matcher.readAllStandardError())));
    }

    // Step 3: COLMAP mapper (增量式 SfM)
    reportProgress(0.6, "running_sfm_reconstruction", 0);

    QProcess mapper;
    mapper.start("colmap", {
        "mapper",
        "--database_path", dbPath,
        "--image_path", framesDir,
        "--output_path", sfmDir
    });
    mapper.waitForFinished(1200000); // 20 分钟超时

    if (mapper.exitCode() != 0) {
        return StageResult::failure(
            QStringLiteral("COLMAP 稀疏重建失败。请确保视频绕物体拍摄了足够的角度。"
                           "避免纯色背景和镜面反射物体。\n详情: %1")
            .arg(QString(mapper.readAllStandardError())));
    }

    // 检查输出
    QString sparseDir = sfmDir + "/0"; // COLMAP 默认第一个模型
    bool found = false;
    if (QDir(sparseDir).exists()) {
        if (QFile::exists(sparseDir + "/cameras.bin") ||
            QFile::exists(sparseDir + "/cameras.txt")) {
            found = true;
        }
    }
    // 尝试其他子目录
    if (!found) {
        QDir sd(sfmDir);
        QStringList subs = sd.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
        for (const auto& sub : subs) {
            if (QFile::exists(sfmDir + "/" + sub + "/cameras.bin") ||
                QFile::exists(sfmDir + "/" + sub + "/cameras.txt")) {
                found = true;
                break;
            }
        }
    }
    if (!found) {
        return StageResult::failure(
            QStringLiteral("COLMAP 未能成功生成相机参数。视频中可能缺少足够的运动视差。"));
    }

    reportProgress(1.0, "sfm_complete", 0);
    return StageResult::success();
}
