// /Share/GenerateModel/src/main.cpp
#include <QGuiApplication>
#include "app/Application.h"

int main(int argc, char *argv[])
{
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    Application app(argc, argv);
    return app.exec();
}
