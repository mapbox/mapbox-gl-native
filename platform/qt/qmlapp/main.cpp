#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <qqml.h>

#include <QQuickMapboxGL>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    qmlRegisterType<QQuickMapboxGL>("QQuickMapboxGL", 1, 0, "QQuickMapboxGL");

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    return app.exec();
}
