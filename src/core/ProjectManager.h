// /Share/GenerateModel/src/core/ProjectManager.h
#pragma once

#include <QString>
#include <QJsonObject>
#include <QDir>

class ProjectManager {
public:
    /// 创建新项目目录结构，返回项目根路径
    static QString createProject(const QString& baseDir, const QString& projectName);

    /// 加载已有项目的 settings.json
    static QJsonObject loadSettings(const QString& projectDir);

    /// 保存 settings.json (合并写入，不覆盖已有 key 除非传入新值)
    static void saveSettings(const QString& projectDir, const QJsonObject& settings);

    /// 更新 checkpoint — 标记某阶段完成
    static void markStageComplete(const QString& projectDir, const QString& stageName, bool done);

    /// 检查某阶段是否已完成
    static bool isStageComplete(const QString& projectDir, const QString& stageName);

    /// 获取最后完成的阶段名，无则返回空字符串
    static QString lastCompletedStage(const QString& projectDir);

    /// 重置 checkpoint — 清空阶段状态，保留配置
    static void resetCheckpoints(const QString& projectDir);

    /// 删除项目所有中间产物 (frames/, sfm/, mesh/, textures/, logs/)，保留 source/ 和 config/
    static void clearIntermediateFiles(const QString& projectDir);

    /// 确保目录存在
    static void ensureDir(const QString& path);

private:
    static QString settingsPath(const QString& projectDir);
    static const QStringList kSubDirs;
};
