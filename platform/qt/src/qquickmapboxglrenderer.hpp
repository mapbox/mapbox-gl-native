#pragma once

#include "qmapbox.hpp"
#include "qmapboxgl.hpp"

#include <QObject>
#include <QQuickFramebufferObject>
#include <QScopedPointer>

class QGeoCoordinate;
class QOpenGLFramebufferObject;
class QSize;

class QQuickMapboxGLRenderer : public QObject, public QQuickFramebufferObject::Renderer
{
    Q_OBJECT

public:
    QQuickMapboxGLRenderer();
    virtual ~QQuickMapboxGLRenderer();

    virtual QOpenGLFramebufferObject * createFramebufferObject(const QSize &);

    virtual void render();
    virtual void synchronize(QQuickFramebufferObject *);

signals:
    void centerChanged(const QGeoCoordinate &);

private:
    bool m_initialized = false;
    qreal m_pixelRatio = 1;

    QScopedPointer<QMapboxGL> m_map;
};
