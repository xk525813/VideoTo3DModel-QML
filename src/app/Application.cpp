// /Share/GenerateModel/src/app/Application.cpp
#include "Application.h"
#include <QQmlContext>
#include <QWindow>
#include <QDebug>

Application::Application(int& argc, char** argv)
    : QGuiApplication(argc, argv)
{
    setApplicationName(appName());
    setOrganizationName(orgName());
    setApplicationVersion(version());

    init();
}

Application::~Application()
{
    if (m_server) {
        m_server->close();
    }
}

void Application::init()
{
    // ── 单实例检测 ──
    QLocalSocket probe;
    probe.connectToServer(kServerName);
    if (probe.waitForConnected(500)) {
        // 已有主实例 → 通知激活 → 退出
        probe.write("activate");
        probe.waitForBytesWritten(500);
        probe.disconnectFromServer();
        qDebug() << "Another instance is running. Activating it, then exit.";
        // 直接退出事件循环（main 中通过 shouldContinue 判断）
        return;
    }

    // 我们是主实例 → 启动监听
    QLocalServer::removeServer(kServerName);

    m_server = new QLocalServer(this);
    if (!m_server->listen(kServerName)) {
        qWarning() << "QLocalServer::listen failed:" << m_server->errorString();
        // 降级运行（不阻塞）
    } else {
        connect(m_server, &QLocalServer::newConnection,
                this, &Application::onNewConnection);
        qDebug() << "Primary instance listening on:" << kServerName;
    }

    setupQml();
}

bool Application::shouldContinue() const
{
    // QML 引擎加载了界面 → 是主实例
    return !m_engine.rootObjects().isEmpty();
}

void Application::onNewConnection()
{
    while (m_server->hasPendingConnections()) {
        auto* client = m_server->nextPendingConnection();
        connect(client, &QLocalSocket::readyRead, this, [this, client]() {
            client->readAll();
            // 激活主窗口
            const auto windows = m_engine.rootObjects();
            for (auto* obj : windows) {
                if (auto* win = qobject_cast<QWindow*>(obj)) {
                    win->show();
                    win->raise();
                    win->requestActivate();
                }
            }
            client->deleteLater();
        });
    }
}

void Application::setupQml()
{
    qmlRegisterType<PipelineBridge>("video3d.pipeline", 1, 0, "PipelineBridge");

    m_engine.rootContext()->setContextProperty("_appVersion", version());

    QObject::connect(&m_engine, &QQmlApplicationEngine::objectCreationFailed,
                     this, []() { QCoreApplication::exit(-1); });

    m_engine.load(QUrl(QStringLiteral("qrc:/ui/MainWindow.qml")));
}
