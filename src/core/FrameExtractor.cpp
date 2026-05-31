// /Share/GenerateModel/src/core/FrameExtractor.cpp
#include "FrameExtractor.h"
#include "ProjectManager.h"
#include <QProcess>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QDebug>

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>

StageResult FrameExtractor::execute(const QString& projectDir,
                                     const QJsonObject& config) {
    QString sourceDir = projectDir + "/source";
    QString framesDir = projectDir + "/frames";

    // 查找源视频
    QDir sd(sourceDir);
    QStringList videos = sd.entryList({"*.mp4", "*.MP4", "*.mov", "*.MOV",
                                        "*.avi", "*.AVI", "*.mkv", "*.MKV"},
                                       QDir::Files);
    if (videos.isEmpty()) {
        return StageResult::failure(
            QStringLiteral("source 目录中未找到视频文件。请先将视频放入 %1")
            .arg(sourceDir));
    }
    QString videoPath = sd.absoluteFilePath(videos.first());

    int maxDim = config["pipelineStrategy"].toObject()["maxFrameDimension"].toInt(1920);

    // 使用 FFmpeg 提取帧
    QProcess ffmpeg;
    QString ffmpegPath = config["tools"].toObject()["ffmpeg"].toString("ffmpeg");
    ffmpeg.start(ffmpegPath, {
        "-i", videoPath,
        "-vf", QString("scale='min(%1,iw)':'min(%1,ih)':force_original_aspect_ratio=decrease").arg(maxDim),
        "-qscale:v", "2",
        "-frame_pts", "1",
        framesDir + "/%04d.png"
    });
    ffmpeg.waitForFinished(300000); // 5 分钟超时

    if (ffmpeg.exitCode() != 0) {
        return StageResult::failure(
            QStringLiteral("FFmpeg 帧提取失败: %1")
            .arg(QString(ffmpeg.readAllStandardError())));
    }

    // 构建帧清单 (manifest.json)
    QDir fd(framesDir);
    QStringList pngs = fd.entryList({"*.png"}, QDir::Files, QDir::Name);
    QJsonObject manifest;
    QJsonArray frameList;
    int kept = 0;

    for (const auto& png : pngs) {
        QString fullPath = fd.absoluteFilePath(png);
        double score = blurScore(fullPath);

        QJsonObject entry;
        entry["file"] = png;
        entry["blurScore"] = score;
        frameList.append(entry);
        kept++;
    }

    if (kept < 5) {
        return StageResult::failure(
            QStringLiteral("提取的有效帧数不足 (仅 %1 帧)。请确保视频长度至少 5 秒且有充分运动。")
            .arg(kept));
    }

    manifest["totalFrames"] = kept;
    manifest["frames"] = frameList;
    manifest["videoSource"] = videos.first();

    QFile mf(framesDir + "/manifest.json");
    if (!mf.open(QIODevice::WriteOnly)) {
        return StageResult::failure(
            QStringLiteral("无法写入 manifest.json: %1").arg(mf.errorString()));
    }
    mf.write(QJsonDocument(manifest).toJson(QJsonDocument::Indented));

    return StageResult::success();
}

double FrameExtractor::blurScore(const QString& imagePath) {
    cv::Mat img = cv::imread(imagePath.toStdString(), cv::IMREAD_GRAYSCALE);
    if (img.empty()) return 0.0;

    cv::Mat laplacian;
    cv::Laplacian(img, laplacian, CV_64F);
    cv::Scalar mean, stddev;
    cv::meanStdDev(laplacian, mean, stddev);
    double variance = stddev.val[0] * stddev.val[0];

    // 归一化: 方差 100+ → 清晰, 10- → 模糊
    return std::min(variance / 100.0, 1.0);
}
