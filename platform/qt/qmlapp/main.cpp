#include <QMapbox>

#include <QGuiApplication>
#include <QIcon>
#include <QQmlApplicationEngine>
#include <qqml.h>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

#if QT_VERSION >= 0x050300
    app.setWindowIcon(QIcon(":icon.png"));
#endif

    // Exposes the QQuickMapboxGL module so we
    // can do `import QQuickMapboxGL 1.0`.
    QMapbox::registerTypes();

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    return app.exec();
}
