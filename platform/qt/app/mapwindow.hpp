#ifndef MAPWINDOW_H
#define MAPWINDOW_H

#include <QGLWidget>
#include <QMapboxGL>

class QKeyEvent;
class QMouseEvent;
class QWheelEvent;

class MapWindow : public QGLWidget
{
public:
    MapWindow(const QMapboxGLSettings &);

private:
    void changeStyle();

    // QGLWidget implementation.
    void keyPressEvent(QKeyEvent *ev) final;
    void mousePressEvent(QMouseEvent *ev) final;
    void mouseMoveEvent(QMouseEvent *ev) final;
    void wheelEvent(QWheelEvent *ev) final;
    void resizeGL(int w, int h) final;
    void paintGL() final;

    QPointF m_lastPos;

    QMapboxGL m_map;
};

#endif
