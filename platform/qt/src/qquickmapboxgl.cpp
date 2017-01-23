#include "qquickmapboxgl.hpp"

#include "qquickmapboxglmapparameter.hpp"
#include "qquickmapboxglrenderer.hpp"

#include <mbgl/util/constants.hpp>

#include <QDebug>
#include <QQuickItem>
#include <QRegularExpression>
#include <QString>
#include <QtGlobal>
#include <QQmlListProperty>
#include <QJSValue>

namespace {

static const QRegularExpression s_camelCase {"([a-z0-9])([A-Z])"};
static const QStringList s_parameterTypes = QStringList()
    << "style" << "paint" << "layout" << "layer" << "source" << "filter" << "image"
    << "bearing" << "pitch";

} // namespace

QQuickMapboxGL::QQuickMapboxGL(QQuickItem *parent_)
    : QQuickFramebufferObject(parent_)
{
}

QQuickMapboxGL::~QQuickMapboxGL()
{
}

QQuickFramebufferObject::Renderer *QQuickMapboxGL::createRenderer() const
{
    return new QQuickMapboxGLRenderer;
}

void QQuickMapboxGL::setPlugin(QDeclarativeGeoServiceProvider *)
{
    m_error = QGeoServiceProvider::NotSupportedError;
    m_errorString = "QQuickMapboxGL does not support plugins.";
    emit errorChanged();
}

QDeclarativeGeoServiceProvider *QQuickMapboxGL::plugin() const
{
    return nullptr;
}

void QQuickMapboxGL::componentComplete()
{
    QQuickFramebufferObject::componentComplete();

    for (const auto& param : m_parameters) {
        processMapParameter(param);
    }
}

void QQuickMapboxGL::setMinimumZoomLevel(qreal zoom)
{
    zoom = qMax(mbgl::util::MIN_ZOOM, zoom);
    zoom = qMin(m_maximumZoomLevel, zoom);

    if (m_minimumZoomLevel == zoom) {
        return;
    }

    m_minimumZoomLevel = zoom;
    setZoomLevel(m_zoomLevel); // Constrain.

    emit minimumZoomLevelChanged();
}

qreal QQuickMapboxGL::minimumZoomLevel() const
{
    return m_minimumZoomLevel;
}

void QQuickMapboxGL::setMaximumZoomLevel(qreal zoom)
{
    zoom = qMin(mbgl::util::MAX_ZOOM, zoom);
    zoom = qMax(m_minimumZoomLevel, zoom);

    if (m_maximumZoomLevel == zoom) {
        return;
    }

    m_maximumZoomLevel = zoom;
    setZoomLevel(m_zoomLevel); // Constrain.

    emit maximumZoomLevelChanged();
}

qreal QQuickMapboxGL::maximumZoomLevel() const
{
    return m_maximumZoomLevel;
}

void QQuickMapboxGL::setZoomLevel(qreal zoom)
{
    zoom = qMin(m_maximumZoomLevel, zoom);
    zoom = qMax(m_minimumZoomLevel, zoom);

    if (m_zoomLevel == zoom) {
        return;
    }

    m_zoomLevel = zoom;

    m_syncState |= ZoomNeedsSync;
    update();

    emit zoomLevelChanged(m_zoomLevel);
}

qreal QQuickMapboxGL::zoomLevel() const
{
    return m_zoomLevel;
}

void QQuickMapboxGL::setCenter(const QGeoCoordinate &coordinate)
{
    if (m_center == coordinate) {
        return;
    }

    m_center = coordinate;

    m_syncState |= CenterNeedsSync;
    update();

    emit centerChanged(m_center);
}

QGeoCoordinate QQuickMapboxGL::center() const
{
    return m_center;
}

QGeoServiceProvider::Error QQuickMapboxGL::error() const
{
    return m_error;
}

QString QQuickMapboxGL::errorString() const
{
    return m_errorString;
}

void QQuickMapboxGL::setVisibleRegion(const QGeoShape &shape)
{
    m_visibleRegion = shape;
}

QGeoShape QQuickMapboxGL::visibleRegion() const
{
    return m_visibleRegion;
}

void QQuickMapboxGL::setCopyrightsVisible(bool)
{
    qWarning() << Q_FUNC_INFO << "Not implemented.";
}

bool QQuickMapboxGL::copyrightsVisible() const
{
    return false;
}

void QQuickMapboxGL::setColor(const QColor &color)
{
    if (m_color == color) {
        return;
    }

    m_color = color;

    StyleProperty change;
    change.type = StyleProperty::Paint;
    change.layer = "background";
    change.property = "background-color";
    change.value = color;
    m_stylePropertyChanges << change;

    update();

    emit colorChanged(m_color);
}

QColor QQuickMapboxGL::color() const
{
    return m_color;
}

void QQuickMapboxGL::pan(int dx, int dy)
{
    m_pan += QPointF(dx, -dy);

    m_syncState |= PanNeedsSync;
    update();
}

void QQuickMapboxGL::onMapChanged(QMapboxGL::MapChange change)
{
    if (change == QMapboxGL::MapChangeDidFinishLoadingStyle) {
        m_styleLoaded = true;
        update();
    }
}

bool QQuickMapboxGL::parseImage(QQuickMapboxGLMapParameter *param)
{
    m_imageChanges << Image {
        param->property("name").toString(),
        QImage(param->property("sprite").toString())
    };
    return true;
}

bool QQuickMapboxGL::parseStyle(QQuickMapboxGLMapParameter *param)
{
    QString url = param->property("url").toString();
    if (m_styleUrl == url) {
        return false;
    }

    // Reload parameters if switching style URLs.
    if (!m_styleUrl.isEmpty()) {
        for (const auto& param_ : m_parameters) {
            if (param_->property("type").toString() == "style") continue;
            processMapParameter(param_);
        }
    }

    m_styleUrl = url;
    m_styleLoaded = false;
    m_syncState |= StyleNeedsSync;

    return true;
}

bool QQuickMapboxGL::parseStyleProperty(QQuickMapboxGLMapParameter *param, const QString &name)
{
    // Ignore meta-properties "type", "layer" and "class".
    if (name == "type" || name == "layer" || name == "class") {
        return false;
    }

    QString formattedName(name);
    formattedName = formattedName.replace(s_camelCase, "\\1-\\2").toLower();

    QVariant value = param->property(name.toLatin1());
    if (value.canConvert<QJSValue>()) {
        value = value.value<QJSValue>().toVariant();
    }

    m_stylePropertyChanges << QQuickMapboxGL::StyleProperty {
        param->property("type").toString().at(0) == 'p' ? StyleProperty::Paint : StyleProperty::Layout,
        param->property("layer").toString(),
        formattedName,
        value,
        param->property("class").toString()
    };
    return true;
}

bool QQuickMapboxGL::parseStyleLayer(QQuickMapboxGLMapParameter *param)
{
    QVariantMap layer;
    layer["id"] = param->property("name");
    layer["source"] = param->property("source");
    layer["type"] = param->property("layerType");
    if (param->property("sourceLayer").isValid()) {
        layer["source-layer"] = param->property("sourceLayer");
    }
    m_layerChanges << layer;
    return true;
}

bool QQuickMapboxGL::parseStyleSource(QQuickMapboxGLMapParameter *param)
{
    QString sourceType = param->property("sourceType").toString();

    QVariantMap source;
    source["id"] = param->property("name");
    source["type"] = sourceType;
    if (sourceType == "vector" || sourceType == "raster") {
        source["url"] = param->property("url");
        m_sourceChanges << source;
    } else if (sourceType == "geojson") {
        auto data = param->property("data").toString();
        if (data.startsWith(':')) {
            QFile geojson(data);
            geojson.open(QIODevice::ReadOnly);
            source["data"] = geojson.readAll();
        } else {
            source["data"] = data.toUtf8();
        }
        m_sourceChanges << source;
    } else {
        m_error = QGeoServiceProvider::UnknownParameterError;
        m_errorString = "Invalid source type: " + sourceType;
        emit errorChanged();
        return false;
    }
    return true;
}

bool QQuickMapboxGL::parseStyleFilter(QQuickMapboxGLMapParameter *param)
{
    QVariantMap filter;
    filter["layer"] = param->property("layer");
    filter["filter"] = param->property("filter");
    m_filterChanges << filter;
    return true;
}

bool QQuickMapboxGL::parseBearing(QQuickMapboxGLMapParameter *param)
{
    qreal angle = param->property("angle").toReal();
    if (m_bearing == angle) return false;
    m_bearing = angle;
    m_syncState |= BearingNeedsSync;
    update();
    return true;
}

bool QQuickMapboxGL::parsePitch(QQuickMapboxGLMapParameter *param)
{
    qreal angle = param->property("angle").toReal();
    angle = qMin(qMax(0., angle), mbgl::util::PITCH_MAX * mbgl::util::RAD2DEG);
    if (m_pitch == angle) return false;
    m_pitch = angle;
    m_syncState |= PitchNeedsSync;
    update();
    return true;
}

void QQuickMapboxGL::processMapParameter(QQuickMapboxGLMapParameter *param)
{
    bool needsUpdate = false;
    switch (s_parameterTypes.indexOf(param->property("type").toString())) {
    case -1:
        m_error = QGeoServiceProvider::UnknownParameterError;
        m_errorString = "Invalid value for property 'type': " + param->property("type").toString();
        emit errorChanged();
        break;
    case 0: // style
        needsUpdate |= parseStyle(param);
        break;
    case 1: // paint
    case 2: // layout
        for (int i = param->propertyOffset(); i < param->metaObject()->propertyCount(); ++i) {
            needsUpdate |= parseStyleProperty(param, QString(param->metaObject()->property(i).name()));
        }
        break;
    case 3: // layer
        needsUpdate |= parseStyleLayer(param);
        break;
    case 4: // source
        needsUpdate |= parseStyleSource(param);
        break;
    case 5: // filter
        needsUpdate |= parseStyleFilter(param);
        break;
    case 6: // image
        needsUpdate |= parseImage(param);
        break;
    case 7: // bearing
        needsUpdate |= parseBearing(param);
        break;
    case 8: // pitch
        needsUpdate |= parsePitch(param);
        break;
    }
    if (needsUpdate) update();
}

void QQuickMapboxGL::onParameterPropertyUpdated(const QString &propertyName)
{
    QQuickMapboxGLMapParameter *param = qobject_cast<QQuickMapboxGLMapParameter *>(sender());
    if (!param) {
        m_error = QGeoServiceProvider::NotSupportedError;
        m_errorString = "QQuickMapboxGL::onParameterPropertyUpdated should be called from a QQuickMapboxGLMapParameter.";
        emit errorChanged();
        return;
    }

    if (propertyName == "type") {
        m_error = QGeoServiceProvider::NotSupportedError;
        m_errorString = "Property 'type' is a write-once.";
        emit errorChanged();
        return;
    }

    bool needsUpdate = false;
    switch (s_parameterTypes.indexOf(param->property("type").toString())) {
    case -1:
        m_error = QGeoServiceProvider::UnknownParameterError;
        m_errorString = "Invalid value for property 'type': " + param->property("type").toString();
        emit errorChanged();
        break;
    case 0: // style
        needsUpdate |= parseStyle(param);
        break;
    case 1: // paint
    case 2: // layout
        needsUpdate |= parseStyleProperty(param, propertyName);
        break;
    case 3: // layer
        needsUpdate |= parseStyleLayer(param);
        break;
    case 4: // source
        needsUpdate |= parseStyleSource(param);
        break;
    case 5: // filter
        needsUpdate |= parseStyleFilter(param);
        break;
    case 6: // image
        needsUpdate |= parseImage(param);
        break;
    case 7: // bearing
        needsUpdate |= parseBearing(param);
        break;
    case 8: // pitch
        needsUpdate |= parsePitch(param);
        break;
    }
    if (needsUpdate) update();
}

void QQuickMapboxGL::appendParameter(QQmlListProperty<QQuickMapboxGLMapParameter> *prop, QQuickMapboxGLMapParameter *param)
{
    QQuickMapboxGL *map = static_cast<QQuickMapboxGL *>(prop->object);
    map->m_parameters.append(param);
    QObject::connect(param, SIGNAL(propertyUpdated(QString)), map, SLOT(onParameterPropertyUpdated(QString)));
}

int QQuickMapboxGL::countParameters(QQmlListProperty<QQuickMapboxGLMapParameter> *prop)
{
    QQuickMapboxGL *map = static_cast<QQuickMapboxGL *>(prop->object);
    return map->m_parameters.count();
}

QQuickMapboxGLMapParameter *QQuickMapboxGL::parameterAt(QQmlListProperty<QQuickMapboxGLMapParameter> *prop, int index)
{
    QQuickMapboxGL *map = static_cast<QQuickMapboxGL *>(prop->object);
    return map->m_parameters[index];
}

void QQuickMapboxGL::clearParameter(QQmlListProperty<QQuickMapboxGLMapParameter> *prop)
{
    QQuickMapboxGL *map = static_cast<QQuickMapboxGL *>(prop->object);
    for (auto param : map->m_parameters) {
        QObject::disconnect(param, SIGNAL(propertyUpdated(QString)), map, SLOT(onParameterPropertyUpdated(QString)));
    }
    map->m_parameters.clear();
}

QQmlListProperty<QQuickMapboxGLMapParameter> QQuickMapboxGL::parameters()
{
    return QQmlListProperty<QQuickMapboxGLMapParameter>(this,
            nullptr,
            appendParameter,
            countParameters,
            parameterAt,
            clearParameter);
}
