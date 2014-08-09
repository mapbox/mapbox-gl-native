#ifndef MBGL_MAP_TRANSFORM
#define MBGL_MAP_TRANSFORM

#include <mbgl/util/time.hpp>
#include <mbgl/map/transform_state.hpp>

#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/uv.hpp>
#include <mbgl/geometry/point.hpp>
#include <mbgl/geometry/lat_lng.hpp>

#include <cstdint>
#include <functional>

namespace mbgl {

class View;

class Transform : private util::noncopyable {
public:
    Transform(View &view);

    // Map view
    // Note: width * ratio does not necessarily equal fb_width
    bool resize(uint16_t width, uint16_t height, float ratio,
                uint16_t fb_width, uint16_t fb_height);

    void setView(const LatLng& center, double zoom, double bearing);

    void setCenter(const LatLng& center);
    LatLng getCenter() const;

    void setZoom(double zoom);
    double getZoom() const;

    void setBearing(double bearing);
    double getBearing() const;

    bool canRotate();
    double getMinZoom() const;
    double getMaxZoom() const;

    Point locationPoint(const LatLng&) const;
    LatLng pointLocation(const Point&) const;

    // Transitions
    void panBy(const Point& delta, timestamp duration);
    void panTo(const LatLng& center, timestamp duration);
    void zoomTo(double zoom, const LatLng& around, timestamp duration);
    void rotateTo(double bearing, const LatLng& around, timestamp duration);
    void easeTo(const LatLng& center, double zoom, double bearing, timestamp duration);
    void flyTo(const LatLng& center, double zoom, double bearing, timestamp duration);

    bool needsTransition() const;
    void updateTransitions(timestamp now);
    void cancelTransitions();

    const TransformState getState() const;

private:
    typedef std::function<void (timestamp)> Transition;

    void constrain(double& scale, double& y) const;
    void timed(const Transition&, timestamp duration);

    View &view;
    std::unique_ptr<uv::rwlock> mtx;
    TransformState state;

    // Limit the amount of zooming possible on the map.
    // TODO: make these modifiable from outside.
    const double min_zoom = 0;
    const double max_zoom = 20;

    Transition transition;
    timestamp start;
    timestamp duration;
};

}

#endif
