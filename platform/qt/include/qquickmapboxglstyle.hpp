#ifndef QQUICKMAPBOXGLSTYLE_H
#define QQUICKMAPBOXGLSTYLE_H

#include <QQuickItem>

class Q_DECL_EXPORT QQuickMapboxGLStyle : public QQuickItem
{
    Q_OBJECT

    Q_PROPERTY(QString url READ url WRITE setUrl NOTIFY urlChanged)
    Q_PROPERTY(QString styleClass READ styleClass WRITE setStyleClass NOTIFY classChanged)

public:
    QQuickMapboxGLStyle(QQuickItem *parent = Q_NULLPTR);
    virtual ~QQuickMapboxGLStyle() {}

    // QQuickItem implementation
    virtual void itemChange(QQuickItem::ItemChange, const QQuickItem::ItemChangeData &);

    void setUrl(const QString &);
    QString url() const;

    void setStyleClass(const QString &);
    QString styleClass() const;

signals:
    void urlChanged(const QString &);
    void classChanged(const QString &);

    void propertyUpdated(const QVariantMap &);

private:
    QString m_url;
    QString m_class;
};

QML_DECLARE_TYPE(QQuickMapboxGLStyle)

#endif // QQUICKMAPBOXGLSTYLE_H
