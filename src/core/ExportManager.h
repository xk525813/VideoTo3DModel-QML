// /Share/GenerateModel/src/core/ExportManager.h
#pragma once

#include "IStage.h"
#include <QStringList>

class ExportManager : public IStage {
public:
    QString name() const override { return "export"; }

    StageResult execute(const QString& projectDir,
                        const QJsonObject& config) override;

    /// 列出可用的导出格式
    static QStringList availableFormats();

    /// 将 OBJ 模型 + 贴图通过 obj2gltf 转换为 GLB
    static StageResult exportWithObj2gltf(const QString& objPath,
                                          const QString& diffusePath,
                                          const QString& outputPath,
                                          bool binary);
};
