// /Share/GenerateModel/src/core/MVSStage.cpp
#include "MVSStage.h"
#include "ProjectManager.h"
#include <QProcess>
#include <QDir>
#include <QFile>
#include <QJsonObject>
#include <QDebug>


StageResult MVSStage::convertCOLMAPtoMVS(const QString& projectDir,
                                            const QJsonObject& tools,
                                            const ProgressCallback& onProgress) {
    if (onProgress) onProgress(0.05, "converting_colmap_to_mvs", 0);

    QString sfmDir = projectDir + "/sfm";
    QString meshDir = projectDir + "/mesh";

    QString colmapPath = tools["colmap"].toString("colmap");
    QString interfacePath = tools["InterfaceCOLMAP"].toString("InterfaceCOLMAP");

    // 查找 COLMAP 模型目录
    QDir sd(sfmDir);
    QStringList subs = sd.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    QString modelDir;
    for (const auto& sub : subs) {
        if (QFile::exists(sfmDir + "/" + sub + "/cameras.bin")) {
            modelDir = sfmDir + "/" + sub;
            break;
        }
    }
    if (modelDir.isEmpty()) {
        return StageResult::failure(
            QStringLiteral("未找到 COLMAP 模型输出，无法继续到 MVS 阶段。"));
    }

    // colmap model_converter: .bin → .txt (OpenMVS 需要文本格式)
    QProcess converter;
    converter.start(colmapPath, {
        "model_converter",
        "--input_path", modelDir,
        "--output_path", meshDir,
        "--output_type", "TXT"
    });
    converter.waitForFinished(60000);

    if (converter.exitCode() != 0) {
        return StageResult::failure(
            QStringLiteral("COLMAP 模型格式转换失败: %1")
            .arg(QString(converter.readAllStandardError())));
    }

    // colmap image_undistorter
    QProcess undistort;
    undistort.start(colmapPath, {
        "image_undistorter",
        "--image_path", projectDir + "/frames",
        "--input_path", modelDir,
        "--output_path", meshDir + "/undistorted",
        "--output_type", "COLMAP"
    });
    undistort.waitForFinished(120000);

    // 生成 OpenMVS .mvs 文件: InterfaceCOLMAP
    QProcess interface;
    interface.start(interfacePath, {
        "-i", meshDir,
        "-o", meshDir + "/scene.mvs",
        "--image-folder", meshDir + "/undistorted/images"
    });
    interface.waitForFinished(60000);

    if (interface.exitCode() != 0) {
        return StageResult::failure(
            QStringLiteral("InterfaceCOLMAP 转换失败，请确认 OpenMVS 工具已正确安装。\n%1")
            .arg(QString(interface.readAllStandardError())));
    }

    return StageResult::success();
}

StageResult MVSStage::execute(const QString& projectDir,
                               const QJsonObject& config, const ProgressCallback& onProgress) {
    QJsonObject tools = config["tools"].toObject();

    // Step 1: 格式转换
    StageResult convResult = convertCOLMAPtoMVS(projectDir, tools, onProgress);
    if (!convResult.ok)
        return convResult;

    QString meshDir = projectDir + "/mesh";
    QString sceneFile = meshDir + "/scene.mvs";
    int denseQuality = config["pipelineStrategy"].toObject()["denseQuality"].toInt(2);

    QString densifyPath = tools["DensifyPointCloud"].toString("DensifyPointCloud");
    QString reconstructPath = tools["ReconstructMesh"].toString("ReconstructMesh");
    QString refinePath = tools["RefineMesh"].toString("RefineMesh");

    // Step 2: DensifyPointCloud
    if (onProgress) onProgress(0.3, "densifying_point_cloud", 0);

    QProcess densify;
    densify.start(densifyPath, {
        "--input-file", sceneFile,
        "--resolution-level", QString::number(denseQuality),
        "--number-views-fuse", QString::number(denseQuality == 3 ? 4 : 2)
    });
    densify.waitForFinished(1800000); // 30 分钟

    if (densify.exitCode() != 0) {
        return StageResult::failure(
            QStringLiteral("稠密点云重建失败。可能需要降级质量参数。\n%1")
            .arg(QString(densify.readAllStandardError())));
    }

    // Step 3: ReconstructMesh
    if (onProgress) onProgress(0.7, "reconstructing_mesh", 0);

    QProcess reconstruct;
    reconstruct.start(reconstructPath, {
        "--input-file", meshDir + "/scene_dense.mvs",
        "--output-file", meshDir + "/model.obj"
    });
    reconstruct.waitForFinished(600000);

    if (reconstruct.exitCode() != 0) {
        return StageResult::failure(
            QStringLiteral("网格重建失败。\n%1")
            .arg(QString(reconstruct.readAllStandardError())));
    }

    // Step 4: RefineMesh (可选)
    if (onProgress) onProgress(0.9, "refining_texture", 0);

    QProcess texture;
    texture.start(refinePath, {
        "--input-file", meshDir + "/scene_dense_mesh.mvs",
        "--output-file", meshDir + "/model_textured.obj"
    });
    texture.waitForFinished(300000);
    // RefineMesh 失败不阻塞，接受无纹理的模型

    if (onProgress) onProgress(1.0, "mvs_complete", 0);
    return StageResult::success();
}
