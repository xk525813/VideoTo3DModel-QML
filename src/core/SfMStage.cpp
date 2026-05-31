// /Share/GenerateModel/src/core/SfMStage.cpp
#include "SfMStage.h"
#include "ProjectManager.h"
#include "LogManager.h"
#include <QProcess>
#include <QDir>
#include <QJsonObject>
#include <QFile>


StageResult SfMStage::execute(const QString& projectDir,
                               const QJsonObject& config, const ProgressCallback& onProgress) {
    QString framesDir = projectDir + "/frames";
    QString sfmDir = projectDir + "/sfm";
    QString dbPath = sfmDir + "/database.db";

    bool useGPU = config["pipelineStrategy"].toObject()["useGPU"].toBool(true);
    QString colmapPath = config["tools"].toObject()["colmap"].toString("colmap");

    // Step 1: COLMAP feature_extractor
    if (onProgress) onProgress(0.1, "extracting_features", 0);

    QProcess extractor;
    extractor.start(colmapPath, {
        "feature_extractor",
        "--database_path", dbPath,
        "--image_path", framesDir,
        "--SiftExtraction.use_gpu", useGPU ? "1" : "0",
        "--SiftExtraction.max_image_size", "2000",
        "--SiftExtraction.max_num_features", "8192"
    });
    extractor.waitForFinished(600000); // 10 分钟超时

    if (extractor.exitCode() != 0) {
        return StageResult::failure(
            QStringLiteral("COLMAP 特征提取失败。视频画面可能纹理不足（如纯色背景）。"
                           "请确保拍摄物体表面有可见纹理，避免大面积纯色。\n详情: %1")
            .arg(QString(extractor.readAllStandardError())));
    }

    // Step 2: COLMAP 特征匹配
    // 先用 sequential_matcher（更稳定），失败再尝试 exhaustive_matcher
    if (onProgress) onProgress(0.4, "matching_features", 0);

    QProcess matcher;
    matcher.start(colmapPath, {
        "sequential_matcher",
        "--database_path", dbPath,
        "--SiftMatching.use_gpu", useGPU ? "1" : "0"
    });
    matcher.waitForFinished(600000);

    if (matcher.exitCode() != 0 || matcher.exitStatus() == QProcess::CrashExit) {
        // 尝试 exhaustive_matcher 作为备选
        LogManager::warn(spdlog::source_loc{__FILE__, __LINE__, __FUNCTION__},
                         "sequential_matcher failed, trying exhaustive_matcher");
        QProcess matcher2;
        matcher2.start(colmapPath, {
            "exhaustive_matcher",
            "--database_path", dbPath,
            "--SiftMatching.use_gpu", useGPU ? "1" : "0"
        });
        matcher2.waitForFinished(600000);
        if (matcher2.exitCode() != 0 || matcher2.exitStatus() == QProcess::CrashExit) {
            return StageResult::failure(
                QStringLiteral("COLMAP 特征匹配失败，可能原因：\n"
                               "  • 视频画面纹理不足（纯色背景、镜面反射）\n"
                               "  • 帧间变化太小（相机移动不够）\n"
                               "  • 提取的有效帧数不够\n"
                               "请尝试：更换纹理更丰富的拍摄物体，增加拍摄角度变化"));

        }
    }

    // Step 3: COLMAP mapper (增量式 SfM)
    if (onProgress) onProgress(0.6, "running_sfm_reconstruction", 0);

    QProcess mapper;
    mapper.start(colmapPath, {
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

    if (onProgress) onProgress(1.0, "sfm_complete", 0);
    return StageResult::success();
}
