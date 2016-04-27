#ifndef QQUICKMAPBOXGLRENDERER_H
#define QQUICKMAPBOXGLRENDERER_H

#include <QScopedPointer>
#include <QQuickFramebufferObject>

class QOpenGLFramebufferObject;
class QSize;
class QMapboxGL;

class QQuickMapboxGLRenderer : public QQuickFramebufferObject::Renderer
{
public:
    QQuickMapboxGLRenderer();
    virtual ~QQuickMapboxGLRenderer();

    virtual QOpenGLFramebufferObject* createFramebufferObject(const QSize& size);

    virtual void render();
    virtual void synchronize(QQuickFramebufferObject *item);

private:
    bool m_initialized = false;

    QScopedPointer<QMapboxGL> m_map;
};

#endif // QQUICKMAPBOXGLRENDERER_H
