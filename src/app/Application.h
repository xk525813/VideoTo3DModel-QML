// /Share/GenerateModel/src/app/Application.h
#pragma once

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QLocalServer>
#include <QLocalSocket>
#include "../bridge/PipelineBridge.h"

/// VideoTo3D 应用程序（单例）
///
/// 继承 QGuiApplication，通过 QLocalServer/QLocalSocket
/// 确保同机只有一个实例运行。第二个实例启动时会激活已有窗口后退出。
class Application : public QGuiApplication {
    Q_OBJECT

public:
    Application(int& argc, char** argv);
    ~Application() override;

    /// false → 已有实例在运行，main() 应立即 return
    bool shouldContinue() const;

    static QString version()  { return "0.1.0"; }
    static QString appName()  { return "VideoTo3D"; }
    static QString orgName()  { return "VideoTo3DTeam"; }

private:
    static constexpr const char* kServerName = "VideoTo3D_SingleInstance";

    QQmlApplicationEngine m_engine;
    QLocalServer*         m_server = nullptr;

    void init();
    void tryBecomePrimary();
    void setupQml();

    /// 已有实例收到激活消息
    void onNewConnection();

    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;
};
