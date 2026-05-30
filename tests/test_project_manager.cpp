// /Share/GenerateModel/tests/test_project_manager.cpp
#include <QtTest>
#include <QTemporaryDir>
#include <QDir>
#include <QFile>
#include <QJsonObject>
#include <QJsonArray>
#include "ProjectManager.h"

class TestProjectManager : public QObject {
    Q_OBJECT

private:
    QTemporaryDir m_tmp;

private slots:
    void initTestCase() {
        QVERIFY(m_tmp.isValid());
    }

    void test_create_project_creates_all_dirs() {
        QString projectDir = ProjectManager::createProject(m_tmp.path(), "TestProject");

        QStringList expectedDirs = {"source", "frames", "sfm", "mesh",
                                     "textures", "output", "config", "logs"};
        for (const auto& d : expectedDirs) {
            QVERIFY2(QDir(projectDir + "/" + d).exists(),
                     qPrintable("Missing dir: " + d));
        }
    }

    void test_create_project_writes_settings() {
        QString projectDir = ProjectManager::createProject(m_tmp.path(), "TestProject2");

        QVERIFY(QFile::exists(projectDir + "/config/settings.json"));

        QJsonObject settings = ProjectManager::loadSettings(projectDir);
        QCOMPARE(settings["projectName"].toString(), QString("TestProject2"));
        QCOMPARE(settings["gpuEnabled"].toBool(), true);
    }

    void test_checkpoint_mark_and_check() {
        QString projectDir = ProjectManager::createProject(m_tmp.path(), "TestProject3");

        QVERIFY(!ProjectManager::isStageComplete(projectDir, "sfm"));

        ProjectManager::markStageComplete(projectDir, "sfm", true);
        QVERIFY(ProjectManager::isStageComplete(projectDir, "sfm"));

        QString last = ProjectManager::lastCompletedStage(projectDir);
        QCOMPARE(last, QString("sfm"));
    }

    void test_checkpoint_unmark() {
        QString projectDir = ProjectManager::createProject(m_tmp.path(), "TestUnmark");

        ProjectManager::markStageComplete(projectDir, "sfm", true);
        QVERIFY(ProjectManager::isStageComplete(projectDir, "sfm"));

        ProjectManager::markStageComplete(projectDir, "sfm", false);
        QVERIFY(!ProjectManager::isStageComplete(projectDir, "sfm"));
    }

    void test_reset_checkpoints() {
        QString projectDir = ProjectManager::createProject(m_tmp.path(), "TestProject4");

        ProjectManager::markStageComplete(projectDir, "sfm", true);
        ProjectManager::markStageComplete(projectDir, "mvs", true);
        QVERIFY(ProjectManager::isStageComplete(projectDir, "sfm"));
        QVERIFY(ProjectManager::isStageComplete(projectDir, "mvs"));

        ProjectManager::resetCheckpoints(projectDir);
        QVERIFY(!ProjectManager::isStageComplete(projectDir, "sfm"));
        QVERIFY(!ProjectManager::isStageComplete(projectDir, "mvs"));
        QCOMPARE(ProjectManager::lastCompletedStage(projectDir), QString(""));
    }

    void test_clear_intermediate_files_keeps_config_and_source() {
        QString projectDir = ProjectManager::createProject(m_tmp.path(), "TestProject5");

        // 创建模拟中间文件
        QFile fakeFrame(projectDir + "/frames/0001.png");
        fakeFrame.open(QIODevice::WriteOnly);
        fakeFrame.write("fake data");
        fakeFrame.close();

        ProjectManager::clearIntermediateFiles(projectDir);

        // config/ 和 source/ 保留
        QVERIFY(QDir(projectDir + "/config").exists());
        QVERIFY(QDir(projectDir + "/source").exists());
        // frames/ 目录存在但文件被删
        QVERIFY(QDir(projectDir + "/frames").exists());
        QVERIFY(!QFile::exists(projectDir + "/frames/0001.png"));
    }
};

QTEST_MAIN(TestProjectManager)
#include "test_project_manager.moc"
