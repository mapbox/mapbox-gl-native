#ifndef QQUICKMAPBOXGL_H
#define QQUICKMAPBOXGL_H

#include <QQuickFramebufferObject>

class QQuickItem;

class Q_DECL_EXPORT QQuickMapboxGL : public QQuickFramebufferObject
{
    Q_OBJECT

public:
    QQuickMapboxGL(QQuickItem *parent = 0);
    virtual ~QQuickMapboxGL();

    // Called from render thread.
    virtual Renderer *createRenderer() const Q_DECL_FINAL;
};

#endif // QQUICKMAPBOXGL_H
