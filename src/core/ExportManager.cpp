// /Share/GenerateModel/src/core/ExportManager.cpp
#include "ExportManager.h"
#include "ProjectManager.h"
#include <QDir>
#include <QFile>
#include <QProcess>
#include <QJsonArray>
#include <QDebug>

QStringList ExportManager::availableFormats() {
    return {"glb", "gltf", "obj"};
}

StageResult ExportManager::execute(const QString& projectDir,
                                    const QJsonObject& config, const ProgressCallback& onProgress) {
    QString format = config["exportFormat"].toString("glb");
    QString meshDir = projectDir + "/mesh";
    QString texturesDir = projectDir + "/textures";
    QString outputDir = projectDir + "/output";

    // 找网格文件
    QString objFile = meshDir + "/model_textured.obj";
    if (!QFile::exists(objFile))
        objFile = meshDir + "/model.obj";
    if (!QFile::exists(objFile))
        return StageResult::failure(QStringLiteral("未找到可导出的网格文件。"));

    QString diffuseTex = texturesDir + "/diffuse.png";
    if (!QFile::exists(diffuseTex))
        diffuseTex.clear();

    QString obj2gltfPath = config["tools"].toObject()["obj2gltf"].toString("obj2gltf");

    if (format == "glb") {
        return exportWithObj2gltf(obj2gltfPath, objFile, diffuseTex, outputDir + "/model.glb", true);
    } else if (format == "gltf") {
        return exportWithObj2gltf(obj2gltfPath, objFile, diffuseTex, outputDir + "/model.gltf", false);
    } else if (format == "obj") {
        // 直接复制
        QFile::copy(objFile, outputDir + "/model.obj");
        // Also copy .mtl if present
        QString mtlFile = objFile;
        mtlFile.replace(".obj", ".mtl");
        if (QFile::exists(mtlFile))
            QFile::copy(mtlFile, outputDir + "/model.mtl");
        if (!diffuseTex.isEmpty())
            QFile::copy(diffuseTex, outputDir + "/diffuse.png");
        return StageResult::success();
    }

    return StageResult::failure(
        QStringLiteral("不支持的导出格式: %1。支持: glb, gltf, obj").arg(format));
}

StageResult ExportManager::exportWithObj2gltf(const QString& obj2gltfPath,
                                               const QString& objPath,
                                               const QString& diffusePath,
                                               const QString& outputPath,
                                               bool binary) {
    QStringList args = {"-i", objPath, "-o", outputPath};
    if (binary)
        args << "-b";
    if (!diffusePath.isEmpty())
        args << "--texture" << diffusePath;

    QProcess obj2gltf;
    obj2gltf.start(obj2gltfPath, args);
    obj2gltf.waitForFinished(120000);

    if (obj2gltf.exitCode() != 0) {
        return StageResult::failure(
            QStringLiteral("导出失败。请确认 obj2gltf 已安装 (npm install -g obj2gltf)。\n%1")
            .arg(QString(obj2gltf.readAllStandardError())));
    }
    return StageResult::success();
}
