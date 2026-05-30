// /Share/GenerateModel/src/core/ProjectManager.cpp
#include "ProjectManager.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QFile>
#include <QDir>
#include <QDateTime>
#include <QDebug>

const QStringList ProjectManager::kSubDirs = {
    "source", "frames", "sfm", "mesh", "textures", "output", "config", "logs"
};

QString ProjectManager::settingsPath(const QString& projectDir) {
    return projectDir + "/config/settings.json";
}

void ProjectManager::ensureDir(const QString& path) {
    QDir().mkpath(path);
}

QString ProjectManager::createProject(const QString& baseDir, const QString& projectName) {
    QString projectDir = baseDir + "/" + projectName;
    for (const auto& sub : kSubDirs) {
        ensureDir(projectDir + "/" + sub);
    }

    QJsonObject defaults;
    defaults["projectName"] = projectName;
    defaults["createdAt"] = QDateTime::currentDateTime().toString(Qt::ISODate);
    defaults["gpuEnabled"] = true;
    defaults["quality"] = "standard";      // low | standard | high
    defaults["exportFormat"] = "glb";
    defaults["textureChannels"] = QJsonArray{"diffuse"};

    QJsonObject checkpoints;
    checkpoints["currentStage"] = "";
    checkpoints["completedStages"] = QJsonArray();
    defaults["checkpoints"] = checkpoints;

    saveSettings(projectDir, defaults);
    return projectDir;
}

QJsonObject ProjectManager::loadSettings(const QString& projectDir) {
    QFile f(settingsPath(projectDir));
    if (!f.open(QIODevice::ReadOnly))
        return {};
    return QJsonDocument::fromJson(f.readAll()).object();
}

void ProjectManager::saveSettings(const QString& projectDir, const QJsonObject& settings) {
    QJsonObject existing = loadSettings(projectDir);
    for (auto it = settings.begin(); it != settings.end(); ++it) {
        existing[it.key()] = it.value();
    }
    QFile f(settingsPath(projectDir));
    f.open(QIODevice::WriteOnly);
    f.write(QJsonDocument(existing).toJson(QJsonDocument::Indented));
}

void ProjectManager::markStageComplete(const QString& projectDir, const QString& stageName, bool done) {
    auto settings = loadSettings(projectDir);
    auto checkpoints = settings["checkpoints"].toObject();
    auto completed = checkpoints["completedStages"].toArray();

    if (done) {
        if (!completed.contains(stageName))
            completed.append(stageName);
        checkpoints["currentStage"] = stageName;
    } else {
        // Remove from completed list
        QJsonArray filtered;
        for (const auto& v : completed) {
            if (v.toString() != stageName)
                filtered.append(v);
        }
        completed = filtered;
    }

    checkpoints["completedStages"] = completed;
    settings["checkpoints"] = checkpoints;
    saveSettings(projectDir, settings);
}

bool ProjectManager::isStageComplete(const QString& projectDir, const QString& stageName) {
    auto settings = loadSettings(projectDir);
    auto checkpoints = settings["checkpoints"].toObject();
    auto completed = checkpoints["completedStages"].toArray();
    return completed.contains(stageName);
}

QString ProjectManager::lastCompletedStage(const QString& projectDir) {
    auto settings = loadSettings(projectDir);
    auto checkpoints = settings["checkpoints"].toObject();
    return checkpoints["currentStage"].toString();
}

void ProjectManager::resetCheckpoints(const QString& projectDir) {
    auto settings = loadSettings(projectDir);
    QJsonObject checkpoints;
    checkpoints["currentStage"] = "";
    checkpoints["completedStages"] = QJsonArray();
    settings["checkpoints"] = checkpoints;
    saveSettings(projectDir, settings);
}

void ProjectManager::clearIntermediateFiles(const QString& projectDir) {
    QStringList dirsToClean = {"frames", "sfm", "mesh", "textures", "logs"};
    for (const auto& d : dirsToClean) {
        QDir dir(projectDir + "/" + d);
        dir.removeRecursively();
        ensureDir(projectDir + "/" + d);
    }
}
