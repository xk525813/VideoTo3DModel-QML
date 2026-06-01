// /Share/GenerateModel/src/app/Application.cpp
#include "Application.h"
#include "../core/LogManager.h"
#include <QQmlContext>
#include <QWindow>

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
    LogManager::shutdown();

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
        probe.write("activate");
        probe.waitForBytesWritten(500);
        probe.disconnectFromServer();
        // 第二实例不初始化日志，直接退出
        return;
    }

    // 设置内建工具库路径
    QString toolsLibPath = QCoreApplication::applicationDirPath() + "/tools/libs";
    if (QDir(toolsLibPath).exists()) {
        QString ldPath = qEnvironmentVariable("LD_LIBRARY_PATH");
        qputenv("LD_LIBRARY_PATH", (toolsLibPath + ":" + ldPath).toUtf8());
    }

    // 初始化日志系统
    LogManager::init(QCoreApplication::applicationDirPath() + "/logs");

    LogManager::info("VideoTo3D v{} 启动", version().toStdString());

    // 启动本地服务器（单实例监听）
    QLocalServer::removeServer(kServerName);

    m_server = new QLocalServer(this);
    if (!m_server->listen(kServerName)) {
        LogManager::warn("QLocalServer 监听失败: {}", m_server->errorString().toStdString());
    } else {
        connect(m_server, &QLocalServer::newConnection,
                this, &Application::onNewConnection);
        LogManager::debug("单实例服务器已启动: {}", std::string(kServerName));
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
