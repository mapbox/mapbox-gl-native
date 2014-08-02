#ifndef MBGL_MAP_TRANSFORM
#define MBGL_MAP_TRANSFORM

#include <mbgl/util/time.hpp>
#include <mbgl/map/transform_state.hpp>

#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/uv.hpp>
#include <mbgl/geometry/point.hpp>
#include <mbgl/geometry/lat_lng.hpp>

#include <cstdint>
#include <cmath>
#include <forward_list>
#include <memory>

namespace mbgl {

class View;
namespace util { class transition; }

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
    void rotateBy(const Point& start, const Point& end, timestamp duration);
    void easeTo(const LatLng& center, double zoom, double bearing, timestamp duration);
    void flyTo(const LatLng& center, double zoom, double bearing, timestamp duration);

    void startPanning();
    void stopPanning();
    void startScaling();
    void stopScaling();
    void startRotating();
    void stopRotating();
    bool needsTransition() const;
    void updateTransitions(timestamp now);
    void cancelTransitions();

    // Transform state
    const TransformState currentState() const;
    const TransformState finalState() const;

private:
    void constrain(double& scale, double& y) const;

    void _clearPanning();
    void _clearScaling();
    void _clearRotating();

    View &view;

    std::unique_ptr<uv::rwlock> mtx;

    // This reflects the current state of the transform, representing the actual position of the
    // map. After calling a transform function with a timer, this will likely remain the same until
    // you render a new frame.
    TransformState current;

    // This reflects the final position of the transform, after all possible transition took place.
    TransformState final;

    // Limit the amount of zooming possible on the map.
    // TODO: make these modifiable from outside.
    const double min_zoom = 0;
    const double max_zoom = 20;

    std::forward_list<std::shared_ptr<util::transition>> transitions;
    std::shared_ptr<util::transition> scale_timeout;
    std::shared_ptr<util::transition> rotate_timeout;
    std::shared_ptr<util::transition> pan_timeout;
};

}

#endif
