#include "qquickmapboxglrenderer.hpp"

#include <QQuickMapboxGL>

#include <QQuickItem>

QQuickMapboxGL::QQuickMapboxGL(QQuickItem *parent_)
    : QQuickFramebufferObject(parent_)
{
}

QQuickMapboxGL::~QQuickMapboxGL()
{
}

QQuickFramebufferObject::Renderer *QQuickMapboxGL::createRenderer() const
{
    return new QQuickMapboxGLRenderer();
}
