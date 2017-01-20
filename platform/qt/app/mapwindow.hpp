#ifndef MAPWINDOW_H
#define MAPWINDOW_H

#include <QGLWidget>
#include <QMapboxGL>
#include <QPropertyAnimation>

class QKeyEvent;
class QMouseEvent;
class QWheelEvent;

class MapWindow : public QGLWidget
{
    Q_OBJECT

public:
    MapWindow(const QMapboxGLSettings &);

    void selfTest();

protected slots:
    void animationValueChanged();
    void animationFinished();

private:
    void changeStyle();
    qreal pixelRatio();

    // QGLWidget implementation.
    void keyPressEvent(QKeyEvent *ev) final;
    void mousePressEvent(QMouseEvent *ev) final;
    void mouseMoveEvent(QMouseEvent *ev) final;
    void wheelEvent(QWheelEvent *ev) final;

    void initializeGL() final;
    void paintGL() final;

    QPointF m_lastPos;

    QMapboxGL m_map;

    QPropertyAnimation m_bearingAnimation;
    QPropertyAnimation m_zoomAnimation;

    unsigned m_animationTicks = 0;
    unsigned m_frameDraws = 0;

    bool m_sourceAdded = false;
};

#endif
