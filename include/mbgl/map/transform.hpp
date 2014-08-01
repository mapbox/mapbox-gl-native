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

    // Position
    void moveBy(const Point&, timestamp duration = 0);
    void setLatLng(const LatLng&, timestamp duration = 0);
    void setLatLngZoom(const LatLng&, double zoom, timestamp duration = 0);
    LatLng getLatLng() const;
    void getLatLngZoom(LatLng& lngLat, double &zoom) const;
    void startPanning();
    void stopPanning();

    // Zoom
    void scaleBy(double ds, timestamp duration = 0);
    void scaleBy(double ds, const Point& center, timestamp duration = 0);
    void setScale(double scale, timestamp duration = 0);
    void setScale(double scale, const Point& center, timestamp duration = 0);
    void setZoom(double zoom, timestamp duration = 0);
    double getZoom() const;
    double getScale() const;
    void startScaling();
    void stopScaling();
    double getMinZoom() const;
    double getMaxZoom() const;

    // Angle
    void rotateBy(const Point& start, const Point& end, timestamp duration = 0);
    void setAngle(double angle, timestamp duration = 0);
    void setAngle(double angle, double cx, double cy);
    double getAngle() const;
    void startRotating();
    void stopRotating();
    bool canRotate();

    // Transitions
    bool needsTransition() const;
    void updateTransitions(timestamp now);
    void cancelTransitions();

    // Transform state
    const TransformState currentState() const;
    const TransformState finalState() const;

private:
    Point centerPoint() const;
    Point project(const LatLng&, const double scale) const;
    LatLng unproject(const Point&) const;

    // Functions prefixed with underscores will *not* perform any locks. It is the caller's
    // responsibility to lock this object.
    void _moveBy(const Point& delta, timestamp duration = 0);
    void _setScale(double scale, const Point& center, timestamp duration = 0);
    void _setScaleXY(double new_scale, const Point& center, timestamp duration = 0);
    void _setAngle(double angle, timestamp duration = 0);
    void _clearPanning();
    void _clearRotating();
    void _clearScaling();

    void constrain(double& scale, double& y) const;

private:
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
    const double min_scale = std::pow(2, 0);
    const double max_scale = std::pow(2, 18);

    std::forward_list<std::shared_ptr<util::transition>> transitions;
    std::shared_ptr<util::transition> scale_timeout;
    std::shared_ptr<util::transition> rotate_timeout;
    std::shared_ptr<util::transition> pan_timeout;
};

}

#endif
