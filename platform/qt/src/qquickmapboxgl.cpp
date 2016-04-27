#include "qquickmapboxglrenderer.hpp"

#include <QQuickMapboxGL>

#include <QQuickItem>

QQuickMapboxGL::QQuickMapboxGL(QQuickItem *parent_)
    : QQuickFramebufferObject(parent_)
{
#if QT_VERSION >= 0x050600
    // FIXME: https://github.com/mapbox/mapbox-gl-native/issues/4866
    setMirrorVertically(true);
#endif
}

QQuickMapboxGL::~QQuickMapboxGL()
{
}

QQuickFramebufferObject::Renderer *QQuickMapboxGL::createRenderer() const
{
    return new QQuickMapboxGLRenderer();
}
