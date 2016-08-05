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
    enum Type {
        LayoutType = 0,
        PaintType,
    };

    virtual ~QQuickMapboxGLStyleProperty() {}

    void setLayer(const QString &);
    QString layer() const;

    void setProperty(const QString &);
    QString property() const;

    void setValue(const QVariant &);
    QVariant value() const;

public slots:
    void checkUpdated();

signals:
    void layerChanged(const QString &);
    void propertyChanged(const QString &);
    void valueChanged(const QVariant &);
    void updated(const QVariantMap& params);

protected:
    QQuickMapboxGLStyleProperty(QQuickItem *parent, Type);

    QVariantMap m_map;
};

class Q_DECL_EXPORT QQuickMapboxGLLayoutStyleProperty : public QQuickMapboxGLStyleProperty
{
public:
    QQuickMapboxGLLayoutStyleProperty(QQuickItem *parent = 0);
    virtual ~QQuickMapboxGLLayoutStyleProperty() {}
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
};

#endif // QQUICKMAPBOXGLSTYLEPROPERTY_H
