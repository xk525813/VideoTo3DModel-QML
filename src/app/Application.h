// /Share/GenerateModel/src/app/Application.h
#pragma once

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "../bridge/PipelineBridge.h"

class Application {
public:
    Application(int& argc, char** argv)
        : m_app(argc, argv)
    {
        // 注册 C++ 类型到 QML
        qmlRegisterType<PipelineBridge>("video3d.pipeline", 1, 0, "PipelineBridge");

        m_engine.rootContext()->setContextProperty("_appVersion", "0.1.0");

        const QUrl url(QStringLiteral("qrc:/ui/MainWindow.qml"));
        m_engine.load(url);
    }

    int exec() { return m_app.exec(); }

private:
    QGuiApplication m_app;
    QQmlApplicationEngine m_engine;
};
