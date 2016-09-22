#pragma once

#include <QObject>
#include <QQuickFramebufferObject>
#include <QScopedPointer>

#include <QMapboxGL>

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

    QScopedPointer<QMapboxGL> m_map;
};
