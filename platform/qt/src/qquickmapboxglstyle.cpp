#include <QQuickMapboxGLStyle>
#include <QQuickMapboxGLStyleProperty>

QQuickMapboxGLStyle::QQuickMapboxGLStyle(QQuickItem *parent)
    : QQuickItem(parent)
{
}

void QQuickMapboxGLStyle::itemChange(QQuickItem::ItemChange change, const QQuickItem::ItemChangeData &value)
{
    QQuickItem::itemChange(change, value);

    switch (change) {
    case QQuickItem::ItemChildAddedChange:
        if (QQuickMapboxGLStyleProperty *property = qobject_cast<QQuickMapboxGLStyleProperty *>(value.item)) {
            connect(property, SIGNAL(updated(QVariantMap)), this, SIGNAL(propertyUpdated(QVariantMap)));
            connect(this, SIGNAL(urlChanged(QString)), property, SLOT(checkUpdated()));
        }
        break;
    case QQuickItem::ItemChildRemovedChange:
        if (QQuickMapboxGLStyleProperty *property = qobject_cast<QQuickMapboxGLStyleProperty *>(value.item)) {
            disconnect(property, SIGNAL(updated(QVariantMap)), this, SIGNAL(propertyUpdated(QVariantMap)));
            disconnect(this, SIGNAL(urlChanged(QString)), property, SLOT(checkUpdated()));
        }
        break;
    default:
        break;
    }
}

void QQuickMapboxGLStyle::setUrl(const QString &url)
{
    if (url == m_url) {
        return;
    }

    m_url = url;
    emit urlChanged(url);
}

QString QQuickMapboxGLStyle::url() const
{
    return m_url;
}

void QQuickMapboxGLStyle::setStyleClass(const QString &styleClass)
{
    if (styleClass == m_class) {
        return;
    }

    m_class = styleClass;
    emit classChanged(styleClass);
}

QString QQuickMapboxGLStyle::styleClass() const
{
    return m_class;
}
