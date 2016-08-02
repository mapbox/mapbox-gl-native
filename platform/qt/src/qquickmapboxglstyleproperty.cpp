#include <QQuickMapboxGLStyleProperty>
#include <QQuickMapboxGL>

QQuickMapboxGLStyleProperty::QQuickMapboxGLStyleProperty(QQuickItem *parent_)
    : QQuickItem(parent_)
{
}

QQuickMapboxGLLayoutStyleProperty::QQuickMapboxGLLayoutStyleProperty(QQuickItem *parent_)
    : QQuickMapboxGLStyleProperty(parent_)
{
}

QQuickMapboxGLPaintStyleProperty::QQuickMapboxGLPaintStyleProperty(QQuickItem *parent_)
    : QQuickMapboxGLStyleProperty(parent_)
{
}

void QQuickMapboxGLLayoutStyleProperty::updateParent()
{
    if (m_map.value("layer").isNull() || m_map.value("property").isNull() || m_map.value("value").isNull()) {
        return;
    }

    QQuickMapboxGL *map = qobject_cast<QQuickMapboxGL *>(parentItem());
    if (map) {
        map->setLayoutProperty(layer(), property(), value());
    } else {
        qWarning() << "Style property requires QQuickMapboxGL as parent item.";
    }
}

void QQuickMapboxGLPaintStyleProperty::updateParent()
{
    if (m_map.value("layer").isNull() || m_map.value("property").isNull() || m_map.value("value").isNull()) {
        return;
    }

    QQuickMapboxGL *map = qobject_cast<QQuickMapboxGL *>(parentItem());
    if (map) {
        map->setPaintProperty(layer(), property(), value(), styleClass());
    } else {
        qWarning() << "Style property requires QQuickMapboxGL as parent item.";
    }
}

void QQuickMapboxGLStyleProperty::itemChange(QQuickItem::ItemChange change, const QQuickItem::ItemChangeData &data)
{
    QQuickItem::itemChange(change, data);

    if (change == QQuickItem::ItemParentHasChanged) {
        updateParent();
    }
}

void QQuickMapboxGLStyleProperty::setLayer(const QString &layer)
{
    if (m_map.value("layer").toString() == layer) {
        return;
    }

    m_map["layer"] = layer;
    emit layerChanged(layer);
    updateParent();
}

QString QQuickMapboxGLStyleProperty::layer() const
{
    return m_map.value("layer").toString();
}

void QQuickMapboxGLStyleProperty::setProperty(const QString &property)
{
    if (m_map.value("property").toString() == property) {
        return;
    }

    m_map["property"] = property;
    emit propertyChanged(property);
    updateParent();
}

QString QQuickMapboxGLStyleProperty::property() const
{
    return m_map.value("property").toString();
}

void QQuickMapboxGLStyleProperty::setValue(const QVariant &value)
{
    if (m_map.value("value") == value) {
        return;
    }

    m_map["value"] = value;
    emit valueChanged(value);
    updateParent();
}

QVariant QQuickMapboxGLStyleProperty::value() const
{
    return m_map.value("value");
}

void QQuickMapboxGLPaintStyleProperty::setStyleClass(const QString &styleClass)
{
    if (m_map.value("class").toString() == styleClass) {
        return;
    }

    m_map["class"] = styleClass;
    emit classChanged(styleClass);
    updateParent();
}

QString QQuickMapboxGLPaintStyleProperty::styleClass() const
{
    return m_map.value("class").toString();
}
