#include "qquickmapboxglmapparameter.hpp"

#include <QByteArray>
#include <QMetaObject>
#include <QMetaProperty>
#include <QSignalMapper>

QQuickMapboxGLMapParameter::QQuickMapboxGLMapParameter(QObject *parent)
    : QObject(parent)
    , m_metaPropertyOffset(metaObject()->propertyCount())
{
}

void QQuickMapboxGLMapParameter::componentComplete()
{
    for (int i = m_metaPropertyOffset; i < metaObject()->propertyCount(); ++i) {
        QMetaProperty property = metaObject()->property(i);

        if (!property.hasNotifySignal()) {
            return;
        }

        auto mapper = new QSignalMapper(this);
        mapper->setMapping(this, i);

        const QByteArray signalName = '2' + property.notifySignal().methodSignature();
        QObject::connect(this, signalName, mapper, SLOT(map()));
        QObject::connect(mapper, SIGNAL(mapped(int)), this, SLOT(onPropertyUpdated(int)));
    }
}

void QQuickMapboxGLMapParameter::onPropertyUpdated(int index)
{
    emit propertyUpdated(metaObject()->property(index).name());
}
