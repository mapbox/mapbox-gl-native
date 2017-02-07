#include "mapwindow.hpp"

#include <QApplication>
#include <QColor>
#include <QDebug>
#include <QFile>
#include <QIcon>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QString>

#if QT_VERSION >= 0x050000
#include <QWindow>
#endif

int kAnimationDuration = 10000;


MapWindow::MapWindow(const QMapboxGLSettings &settings)
    : m_settings(settings)
{
    setWindowIcon(QIcon(":icon.png"));
}

void MapWindow::selfTest()
{
    if (m_bearingAnimation) {
        m_bearingAnimation->setDuration(kAnimationDuration);
        m_bearingAnimation->setEndValue(m_map->bearing() + 360 * 4);
        m_bearingAnimation->start();
    }

    if (m_zoomAnimation) {
        m_zoomAnimation->setDuration(kAnimationDuration);
        m_zoomAnimation->setEndValue(m_map->zoom() + 3);
        m_zoomAnimation->start();
    }
}

qreal MapWindow::pixelRatio() {
#if QT_VERSION >= 0x050600
    return devicePixelRatioF();
#elif QT_VERSION >= 0x050000
    return devicePixelRatio();
#else
    return 1;
#endif
}


void MapWindow::animationFinished()
{
    qDebug() << "Animation ticks/s: " <<  m_animationTicks / static_cast<float>(kAnimationDuration) * 1000.;
    qDebug() << "Frame draws/s: " <<  m_frameDraws / static_cast<float>(kAnimationDuration) * 1000.;

    qApp->quit();
}

void MapWindow::animationValueChanged()
{
    m_animationTicks++;
}

void MapWindow::changeStyle()
{
    static uint8_t currentStyleIndex;

    auto& styles = QMapbox::defaultStyles();

    m_map->setStyleUrl(styles[currentStyleIndex].first);
    setWindowTitle(QString("Mapbox GL: ") + styles[currentStyleIndex].second);

    if (++currentStyleIndex == styles.size()) {
        currentStyleIndex = 0;
    }

    m_sourceAdded = false;
}

void MapWindow::keyPressEvent(QKeyEvent *ev)
{
    static const qint64 transitionDuration = 300;

    switch (ev->key()) {
    case Qt::Key_S:
        changeStyle();
        break;
    case Qt::Key_L: {
            if (m_sourceAdded) {
                return;
            }

            m_sourceAdded = true;

            QFile geojson(":source1.geojson");
            geojson.open(QIODevice::ReadOnly);

            // The data source for the route line and markers
            QVariantMap routeSource;
            routeSource["type"] = "geojson";
            routeSource["data"] = geojson.readAll();
            m_map->addSource("routeSource", routeSource);

            // The route case, painted before the route
            QVariantMap routeCase;
            routeCase["id"] = "routeCase";
            routeCase["type"] = "line";
            routeCase["source"] = "routeSource";
            m_map->addLayer(routeCase);

            m_map->setPaintProperty("routeCase", "line-color", QColor("white"));
            m_map->setPaintProperty("routeCase", "line-width", 20.0);
            m_map->setLayoutProperty("routeCase", "line-join", "round");
            m_map->setLayoutProperty("routeCase", "line-cap", "round");

            // The route, painted on top of the route case
            QVariantMap route;
            route["id"] = "route";
            route["type"] = "line";
            route["source"] = "routeSource";
            m_map->addLayer(route);

            m_map->setPaintProperty("route", "line-color", QColor("blue"));
            m_map->setPaintProperty("route", "line-width", 8.0);
            m_map->setLayoutProperty("route", "line-join", "round");
            m_map->setLayoutProperty("route", "line-cap", "round");

            QVariantList lineDashArray;
            lineDashArray.append(1);
            lineDashArray.append(2);

            m_map->setPaintProperty("route", "line-dasharray", lineDashArray);

            // Markers at the beginning and end of the route
            m_map->addImage("label-arrow", QImage(":label-arrow.svg"));
            m_map->addImage("label-background", QImage(":label-background.svg"));

            QVariantMap markerArrow;
            markerArrow["id"] = "markerArrow";
            markerArrow["type"] = "symbol";
            markerArrow["source"] = "routeSource";
            m_map->addLayer(markerArrow);

            m_map->setLayoutProperty("markerArrow", "icon-image", "label-arrow");
            m_map->setLayoutProperty("markerArrow", "icon-size", 0.5);
            m_map->setLayoutProperty("markerArrow", "icon-ignore-placement", true);

            QVariantList arrowOffset;
            arrowOffset.append(0.0);
            arrowOffset.append(-15.0);
            m_map->setLayoutProperty("markerArrow", "icon-offset", arrowOffset);

            QVariantMap markerBackground;
            markerBackground["id"] = "markerBackground";
            markerBackground["type"] = "symbol";
            markerBackground["source"] = "routeSource";
            m_map->addLayer(markerBackground);

            m_map->setLayoutProperty("markerBackground", "icon-image", "label-background");
            m_map->setLayoutProperty("markerBackground", "text-field", "{name}");
            m_map->setLayoutProperty("markerBackground", "icon-text-fit", "both");
            m_map->setLayoutProperty("markerBackground", "icon-ignore-placement", true);
            m_map->setLayoutProperty("markerBackground", "text-ignore-placement", true);
            m_map->setLayoutProperty("markerBackground", "text-anchor", "left");
            m_map->setLayoutProperty("markerBackground", "text-size", 16.0);
            m_map->setLayoutProperty("markerBackground", "text-padding", 0.0);
            m_map->setLayoutProperty("markerBackground", "text-line-height", 1.0);
            m_map->setLayoutProperty("markerBackground", "text-max-width", 8.0);

            QVariantList iconTextFitPadding;
            iconTextFitPadding.append(15.0);
            iconTextFitPadding.append(10.0);
            iconTextFitPadding.append(15.0);
            iconTextFitPadding.append(10.0);
            m_map->setLayoutProperty("markerBackground", "icon-text-fit-padding", iconTextFitPadding);

            QVariantList backgroundOffset;
            backgroundOffset.append(-0.5);
            backgroundOffset.append(-1.5);
            m_map->setLayoutProperty("markerBackground", "text-offset", backgroundOffset);

            m_map->setPaintProperty("markerBackground", "text-color", QColor("white"));

            QVariantList filterExpression;
            filterExpression.append("==");
            filterExpression.append("$type");
            filterExpression.append("Point");

            QVariantList filter;
            filter.append(filterExpression);

            m_map->setFilter("markerArrow", filter);
            m_map->setFilter("markerBackground", filter);

            // Tilt the labels when tilting the map and make them larger
            m_map->setLayoutProperty("road-label-large", "text-size", 30.0);
            m_map->setLayoutProperty("road-label-large", "text-pitch-alignment", "viewport");

            m_map->setLayoutProperty("road-label-medium", "text-size", 30.0);
            m_map->setLayoutProperty("road-label-medium", "text-pitch-alignment", "viewport");

            m_map->setLayoutProperty("road-label-small", "text-pitch-alignment", "viewport");
            m_map->setLayoutProperty("road-label-small", "text-size", 30.0);
        }
        break;
    case Qt::Key_1: {
            if (m_symbolAnnotationId.isNull()) {
                QMapbox::Coordinate coordinate = m_map->coordinate();
                QMapbox::SymbolAnnotation symbol { coordinate, "default_marker" };
                m_map->addAnnotationIcon("default_marker", QImage(":default_marker.svg"));
                m_symbolAnnotationId = m_map->addAnnotation(QVariant::fromValue<QMapbox::SymbolAnnotation>(symbol));
            } else {
                m_map->removeAnnotation(m_symbolAnnotationId.toUInt());
                m_symbolAnnotationId.clear();
            }
        }
        break;
    case Qt::Key_2: {
            if (m_lineAnnotationId.isNull()) {
                QMapbox::Coordinate topLeft     = m_map->coordinateForPixel({ 0, 0 });
                QMapbox::Coordinate bottomRight = m_map->coordinateForPixel({ qreal(size().width()), qreal(size().height()) });
                QMapbox::CoordinatesCollections geometry { { { topLeft, bottomRight } } };
                QMapbox::LineAnnotation line { { QMapbox::ShapeAnnotationGeometry::LineStringType, geometry }, 0.5f, 1.0f, Qt::red };
                m_lineAnnotationId = m_map->addAnnotation(QVariant::fromValue<QMapbox::LineAnnotation>(line));
            } else {
                m_map->removeAnnotation(m_lineAnnotationId.toUInt());
                m_lineAnnotationId.clear();
            }
        }
        break;
    case Qt::Key_3: {
            if (m_fillAnnotationId.isNull()) {
                QMapbox::Coordinate topLeft     = m_map->coordinateForPixel({ 0, 0 });
                QMapbox::Coordinate topRight    = m_map->coordinateForPixel({ 0, qreal(size().height()) });
                QMapbox::Coordinate bottomLeft  = m_map->coordinateForPixel({ qreal(size().width()), 0 });
                QMapbox::Coordinate bottomRight = m_map->coordinateForPixel({ qreal(size().width()), qreal(size().height()) });
                QMapbox::CoordinatesCollections geometry { { { bottomLeft, bottomRight, topRight, topLeft, bottomLeft } } };
                QMapbox::FillAnnotation fill { { QMapbox::ShapeAnnotationGeometry::PolygonType, geometry }, 0.5f, Qt::green, QVariant::fromValue<QColor>(QColor(Qt::black)) };
                m_fillAnnotationId = m_map->addAnnotation(QVariant::fromValue<QMapbox::FillAnnotation>(fill));
            } else {
                m_map->removeAnnotation(m_fillAnnotationId.toUInt());
                m_fillAnnotationId.clear();
            }
        }
        break;
    case Qt::Key_4: {
            if (m_styleSourcedAnnotationId.isNull()) {
                QMapbox::Coordinate topLeft     = m_map->coordinateForPixel({ 0, 0 });
                QMapbox::Coordinate topRight    = m_map->coordinateForPixel({ 0, qreal(size().height()) });
                QMapbox::Coordinate bottomLeft  = m_map->coordinateForPixel({ qreal(size().width()), 0 });
                QMapbox::Coordinate bottomRight = m_map->coordinateForPixel({ qreal(size().width()), qreal(size().height()) });
                QMapbox::CoordinatesCollections geometry { { { bottomLeft, bottomRight, topRight, topLeft, bottomLeft } } };
                QMapbox::StyleSourcedAnnotation styleSourced { { QMapbox::ShapeAnnotationGeometry::PolygonType, geometry }, "water" };
                m_styleSourcedAnnotationId = m_map->addAnnotation(QVariant::fromValue<QMapbox::StyleSourcedAnnotation>(styleSourced));
            } else {
                m_map->removeAnnotation(m_styleSourcedAnnotationId.toUInt());
                m_styleSourcedAnnotationId.clear();
            }
        }
        break;
    case Qt::Key_5: {
            if (m_map->layerExists("circleLayer")) {
                m_map->removeLayer("circleLayer");
                m_map->removeSource("circleSource");
            } else {
                QMapbox::CoordinatesCollections geometry { { { m_map->coordinate() } } };
                QMapbox::Feature feature { QMapbox::Feature::PointType, geometry, {}, {} };

                QVariantMap circleSource;
                circleSource["type"] = "geojson";
                circleSource["data"] = QVariant::fromValue<QMapbox::Feature>(feature);
                m_map->addSource("circleSource", circleSource);

                QVariantMap circle;
                circle["id"] = "circleLayer";
                circle["type"] = "circle";
                circle["source"] = "circleSource";
                m_map->addLayer(circle);

                m_map->setPaintProperty("circleLayer", "circle-radius", 10.0);
                m_map->setPaintProperty("circleLayer", "circle-color", QColor("black"));
            }
        }
        break;
    case Qt::Key_Tab:
        m_map->cycleDebugOptions();
        break;
    case Qt::Key_R: {
        m_map->setTransitionOptions(transitionDuration);
        if (m_map->hasClass("night")) {
            m_map->removeClass("night");
        } else {
            m_map->addClass("night");
        }
    } break;
    default:
        break;
    }

    ev->accept();
}

void MapWindow::mousePressEvent(QMouseEvent *ev)
{
#if QT_VERSION < 0x050000
    m_lastPos = ev->posF();
#else
    m_lastPos = ev->localPos();
#endif

    if (ev->type() == QEvent::MouseButtonPress) {
        if (ev->buttons() == (Qt::LeftButton | Qt::RightButton)) {
            changeStyle();
        }
    }

    if (ev->type() == QEvent::MouseButtonDblClick) {
        if (ev->buttons() == Qt::LeftButton) {
            m_map->scaleBy(2.0, m_lastPos);
        } else if (ev->buttons() == Qt::RightButton) {
            m_map->scaleBy(0.5, m_lastPos);
        }
    }

    ev->accept();
}

void MapWindow::mouseMoveEvent(QMouseEvent *ev)
{
#if QT_VERSION < 0x050000
    QPointF delta = ev->posF() - m_lastPos;
#else
    QPointF delta = ev->localPos() - m_lastPos;
#endif

    if (!delta.isNull()) {
        if (ev->buttons() == Qt::LeftButton && ev->modifiers() & Qt::ShiftModifier) {
            m_map->setPitch(m_map->pitch() - delta.y());
        } else if (ev->buttons() == Qt::LeftButton) {
            m_map->moveBy(delta);
        } else if (ev->buttons() == Qt::RightButton) {
#if QT_VERSION < 0x050000
            m_map->rotateBy(m_lastPos, ev->posF());
#else
            m_map->rotateBy(m_lastPos, ev->localPos());
#endif
        }
    }

#if QT_VERSION < 0x050000
    m_lastPos = ev->posF();
#else
    m_lastPos = ev->localPos();
#endif
    ev->accept();
}

void MapWindow::wheelEvent(QWheelEvent *ev)
{
    if (ev->orientation() == Qt::Horizontal) {
        return;
    }

    float factor = ev->delta() / 1200.;
    if (ev->delta() < 0) {
        factor = factor > -1 ? factor : 1 / factor;
    }

    m_map->scaleBy(1 + factor, ev->pos());
    ev->accept();
}

void MapWindow::initializeGL()
{
    QMapbox::initializeGLExtensions();

    m_map.reset(new QMapboxGL(nullptr, m_settings, size(), pixelRatio()));
    connect(m_map.data(), SIGNAL(needsRendering()), this, SLOT(update()));

    // Set default location to Helsinki.
    m_map->setCoordinateZoom(QMapbox::Coordinate(60.170448, 24.942046), 14);

    QString styleUrl = qgetenv("MAPBOX_STYLE_URL");
    if (styleUrl.isEmpty()) {
        changeStyle();
    } else {
        m_map->setStyleUrl(styleUrl);
        setWindowTitle(QString("Mapbox GL: ") + styleUrl);
    }

    m_bearingAnimation = new QPropertyAnimation(m_map.data(), "bearing");
    m_zoomAnimation = new QPropertyAnimation(m_map.data(), "zoom");

    connect(m_zoomAnimation, SIGNAL(finished()), this, SLOT(animationFinished()));
    connect(m_zoomAnimation, SIGNAL(valueChanged(const QVariant&)), this, SLOT(animationValueChanged()));
}

void MapWindow::paintGL()
{
    m_frameDraws++;
    m_map->resize(size(), size() * pixelRatio());
    m_map->render();
}
