#include <QGuiApplication>
#include <QQmlApplicationEngine>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    // QML 文件将在后续任务中添加
    // const QUrl url(QStringLiteral("qrc:/ui/MainWindow.qml"));
    // engine.load(url);

    // 临时: 直接返回成功 (无 UI)
    return 0;
}
