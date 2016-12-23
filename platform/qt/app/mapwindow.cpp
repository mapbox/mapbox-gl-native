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

            QFile geojson(":source.geojson");
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

            // Markers at the beginning and end of the route
            m_map->addImage("label-arrow", QImage(":label-arrow.svg"));
            m_map->addImage("label-background", QImage(":label-background.svg"));

            QVariantMap makerArrow;
            makerArrow["id"] = "makerArrow";
            makerArrow["type"] = "symbol";
            makerArrow["source"] = "routeSource";
            m_map->addLayer(makerArrow);

            m_map->setLayoutProperty("makerArrow", "icon-image", "label-arrow");
            m_map->setLayoutProperty("makerArrow", "icon-size", 0.5);
            m_map->setLayoutProperty("makerArrow", "icon-ignore-placement", true);

            QVariantList arrowOffset;
            arrowOffset.append(0.0);
            arrowOffset.append(-15.0);
            m_map->setLayoutProperty("makerArrow", "icon-offset", arrowOffset);

            QVariantMap makerBackground;
            makerBackground["id"] = "makerBackground";
            makerBackground["type"] = "symbol";
            makerBackground["source"] = "routeSource";
            m_map->addLayer(makerBackground);

            m_map->setLayoutProperty("makerBackground", "icon-image", "label-background");
            m_map->setLayoutProperty("makerBackground", "text-field", "{name}");
            m_map->setLayoutProperty("makerBackground", "icon-text-fit", "both");
            m_map->setLayoutProperty("makerBackground", "icon-ignore-placement", true);
            m_map->setLayoutProperty("makerBackground", "text-ignore-placement", true);
            m_map->setLayoutProperty("makerBackground", "text-anchor", "left");
            m_map->setLayoutProperty("makerBackground", "text-size", 16.0);
            m_map->setLayoutProperty("makerBackground", "text-padding", 0.0);
            m_map->setLayoutProperty("makerBackground", "text-line-height", 1.0);
            m_map->setLayoutProperty("makerBackground", "text-max-width", 8.0);

            QVariantList iconTextFitPadding;
            iconTextFitPadding.append(15.0);
            iconTextFitPadding.append(10.0);
            iconTextFitPadding.append(15.0);
            iconTextFitPadding.append(10.0);
            m_map->setLayoutProperty("makerBackground", "icon-text-fit-padding", iconTextFitPadding);

            QVariantList backgroundOffset;
            backgroundOffset.append(-0.5);
            backgroundOffset.append(-1.5);
            m_map->setLayoutProperty("makerBackground", "text-offset", backgroundOffset);

            m_map->setPaintProperty("makerBackground", "text-color", QColor("white"));

            QVariantList filterExpression;
            filterExpression.append("==");
            filterExpression.append("$type");
            filterExpression.append("Point");

            QVariantList filter;
            filter.append(filterExpression);

            m_map->setFilter("makerArrow", filter);
            m_map->setFilter("makerBackground", filter);

            // Tilt the labels when tilting the map and make them larger
            m_map->setLayoutProperty("road-label-large", "text-size", 30.0);
            m_map->setLayoutProperty("road-label-large", "text-pitch-alignment", "viewport");

            m_map->setLayoutProperty("road-label-medium", "text-size", 30.0);
            m_map->setLayoutProperty("road-label-medium", "text-pitch-alignment", "viewport");

            m_map->setLayoutProperty("road-label-small", "text-pitch-alignment", "viewport");
            m_map->setLayoutProperty("road-label-small", "text-size", 30.0);
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
