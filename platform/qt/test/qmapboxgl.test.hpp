#include <mbgl/test/util.hpp>

#include <QApplication>
#include <QMapboxGL>

// We're using QGLFramebufferObject, which is only available in Qt 5 and up.
#if QT_VERSION >= 0x050000

#include <QGLWidget>
#include <QGLFramebufferObject>

class QMapboxGLTest : public QObject, public ::testing::Test {
    Q_OBJECT

public:
    QMapboxGLTest();

    void runUntil(QMapboxGL::MapChange);

private:
    QGLWidget widget;
    const QSize size;
    QGLFramebufferObject fbo;

protected:
    QMapboxGLSettings settings;
    QMapboxGL map;

    std::function<void(QMapboxGL::MapChange)> changeCallback;

private slots:
    void onMapChanged(QMapboxGL::MapChange);
    void onNeedsRendering();
};

#endif
