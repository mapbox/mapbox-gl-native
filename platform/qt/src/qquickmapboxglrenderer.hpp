#ifndef QQUICKMAPBOXGLRENDERER_H
#define QQUICKMAPBOXGLRENDERER_H

#include <QObject>
#include <QQuickFramebufferObject>
#include <QScopedPointer>

class QGeoCoordinate;
class QMapboxGL;
class QOpenGLFramebufferObject;
class QSize;

class QQuickMapboxGLRenderer : public QObject, public QQuickFramebufferObject::Renderer
{
    Q_OBJECT

public:
    QQuickMapboxGLRenderer();
    virtual ~QQuickMapboxGLRenderer();

    virtual QOpenGLFramebufferObject* createFramebufferObject(const QSize& size);

    virtual void render();
    virtual void synchronize(QQuickFramebufferObject *item);

signals:
    void centerChanged(const QGeoCoordinate &coordinate);

private:
    bool m_initialized = false;

    QScopedPointer<QMapboxGL> m_map;
};

#endif // QQUICKMAPBOXGLRENDERER_H
