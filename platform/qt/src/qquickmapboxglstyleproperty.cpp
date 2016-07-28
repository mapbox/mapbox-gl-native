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
    if (m_layer.isNull() || m_property.isNull() || m_value.isNull()) {
        return;
    }

    QQuickMapboxGL *map = qobject_cast<QQuickMapboxGL *>(parentItem());
    if (map) {
        map->setLayoutProperty(layer(), property(), m_value);
    } else {
        qWarning() << "Style property requires QQuickMapboxGL as parent item.";
    }
}

void QQuickMapboxGLPaintStyleProperty::updateParent()
{
    if (m_layer.isNull() || m_property.isNull() || m_value.isNull()) {
        return;
    }

    QQuickMapboxGL *map = qobject_cast<QQuickMapboxGL *>(parentItem());
    if (map) {
        map->setPaintProperty(layer(), property(), m_value, styleClass());
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
    if (layer == m_layer.toString()) {
        return;
    }

    m_layer = layer;
    emit layerChanged(layer);
    updateParent();
}

QString QQuickMapboxGLStyleProperty::layer() const
{
    return m_layer.toString();
}

void QQuickMapboxGLStyleProperty::setProperty(const QString &property)
{
    if (property == m_property.toString()) {
        return;
    }

    m_property = property;
    emit propertyChanged(property);
    updateParent();
}

QString QQuickMapboxGLStyleProperty::property() const
{
    return m_property.toString();
}

void QQuickMapboxGLStyleProperty::setValue(const QVariant &value)
{
    if (value == m_value) {
        return;
    }

    m_value = value;
    emit valueChanged(value);
    updateParent();
}

QVariant QQuickMapboxGLStyleProperty::value() const
{
    return m_value;
}

void QQuickMapboxGLPaintStyleProperty::setStyleClass(const QString &styleClass)
{
    if (styleClass == m_class.toString()) {
        return;
    }

    m_class = styleClass;
    emit classChanged(styleClass);
    updateParent();
}

QString QQuickMapboxGLPaintStyleProperty::styleClass() const
{
    return m_class.toString();
}
