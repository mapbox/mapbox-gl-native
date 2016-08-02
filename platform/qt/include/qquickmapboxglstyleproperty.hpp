#ifndef QQUICKMAPBOXGLSTYLEPROPERTY_H
#define QQUICKMAPBOXGLSTYLEPROPERTY_H

#include <QQuickItem>

class Q_DECL_EXPORT QQuickMapboxGLStyleProperty : public QQuickItem
{
    Q_OBJECT

    Q_PROPERTY(QString layer READ layer WRITE setLayer NOTIFY layerChanged)
    Q_PROPERTY(QString property READ property WRITE setProperty NOTIFY propertyChanged)
    Q_PROPERTY(QVariant value READ value WRITE setValue NOTIFY valueChanged)

public:
    virtual ~QQuickMapboxGLStyleProperty() {}

    // QQuickItem implementation
    virtual void itemChange(QQuickItem::ItemChange, const QQuickItem::ItemChangeData &);

    void setLayer(const QString &);
    QString layer() const;

    void setProperty(const QString &);
    QString property() const;

    void setValue(const QVariant &);
    QVariant value() const;

signals:
    void layerChanged(const QString &);
    void propertyChanged(const QString &);
    void valueChanged(const QVariant &);

protected:
    QQuickMapboxGLStyleProperty(QQuickItem *parent);
    virtual void updateParent() = 0;

    QVariantMap m_map;
};

class Q_DECL_EXPORT QQuickMapboxGLLayoutStyleProperty : public QQuickMapboxGLStyleProperty
{
public:
    QQuickMapboxGLLayoutStyleProperty(QQuickItem *parent = 0);
    virtual ~QQuickMapboxGLLayoutStyleProperty() {}

protected:
    virtual void updateParent();
};

class Q_DECL_EXPORT QQuickMapboxGLPaintStyleProperty : public QQuickMapboxGLStyleProperty
{
    Q_OBJECT
    Q_PROPERTY(QString styleClass READ styleClass WRITE setStyleClass NOTIFY classChanged)

public:
    QQuickMapboxGLPaintStyleProperty(QQuickItem *parent = 0);
    virtual ~QQuickMapboxGLPaintStyleProperty() {}

    void setStyleClass(const QString &);
    QString styleClass() const;

signals:
    void classChanged(const QString &);

protected:
    virtual void updateParent();
};

#endif // QQUICKMAPBOXGLSTYLEPROPERTY_H
