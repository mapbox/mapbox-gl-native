#ifndef MAPWINDOW_H
#define MAPWINDOW_H

#include <QMapboxGL>

#include <QtGlobal>

// XXX http://stackoverflow.com/questions/24899558/how-to-check-qt-version-to-include-different-header#comment59591604_29887388
#if QT_VERSION >= 0x050400
#include <QOpenGLWidget>
#else
#include <QGLWidget>
#endif // QT_VERSION

#include <QPropertyAnimation>
#include <QScopedPointer>

class QKeyEvent;
class QMouseEvent;
class QWheelEvent;

#if QT_VERSION >= 0x050400
class MapWindow : public QOpenGLWidget
#else
class MapWindow : public QGLWidget
#endif // QT_VERSION
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

    // QWidget implementation.
    void keyPressEvent(QKeyEvent *ev) final;
    void mousePressEvent(QMouseEvent *ev) final;
    void mouseMoveEvent(QMouseEvent *ev) final;
    void wheelEvent(QWheelEvent *ev) final;

    // Q{,Open}GLWidget implementation.
    void initializeGL() final;
    void paintGL() final;

    QPointF m_lastPos;

    QMapboxGLSettings m_settings;
    QScopedPointer<QMapboxGL> m_map;

    QPropertyAnimation *m_bearingAnimation;
    QPropertyAnimation *m_zoomAnimation;

    unsigned m_animationTicks = 0;
    unsigned m_frameDraws = 0;

    QVariant m_symbolAnnotationId;
    QVariant m_lineAnnotationId;
    QVariant m_fillAnnotationId;

    bool m_sourceAdded = false;
};

#endif
