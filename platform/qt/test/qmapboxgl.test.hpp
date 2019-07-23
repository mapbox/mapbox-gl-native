#include <QMapboxGL>

#include <QGLWidget>
#include <QGLFramebufferObject>

#include <gtest/gtest.h>

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
