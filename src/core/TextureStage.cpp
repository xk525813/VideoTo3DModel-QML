#include "TextureStage.h"
#include "ProjectManager.h"
#include <QDir>
#include <QFile>
#include <QDebug>

StageResult TextureStage::execute(const QString& projectDir,
                                   const QJsonObject& config, const ProgressCallback& onProgress) {
    QString meshDir = projectDir + "/mesh";
    QString texturesDir = projectDir + "/textures";

    // 检查 OpenMVS 是否已生成带纹理的模型
    QString texturedObj = meshDir + "/model_textured.obj";
    QString basicObj = meshDir + "/model.obj";
    QString objToUse;

    if (QFile::exists(texturedObj)) {
        objToUse = texturedObj;
    } else if (QFile::exists(basicObj)) {
        objToUse = basicObj;
    } else {
        return StageResult::failure(
            QStringLiteral("未找到网格文件。请确认 MVS 阶段已成功完成。"));
    }

    // 检查是否有伴随的纹理贴图文件
    QString possibleTexture = meshDir + "/model_textured.png";
    QString possibleTextureMaterial = meshDir + "/model_textured_material_0_map_Kd.png";

    if (QFile::exists(possibleTextureMaterial)) {
        QString destPath = texturesDir + "/diffuse.png";
        QFile::copy(possibleTextureMaterial, destPath);
    } else if (QFile::exists(possibleTexture)) {
        QString destPath = texturesDir + "/diffuse.png";
        QFile::copy(possibleTexture, destPath);
    } else {
        // 无纹理 — 在日志中记录，不阻塞
        QFile log(projectDir + "/logs/texture.log");
        log.open(QIODevice::WriteOnly | QIODevice::Append);
        log.write("No texture map generated. Model is geometry-only.\n");
        log.close();
    }

    return StageResult::success();
}
