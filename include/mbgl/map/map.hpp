#ifndef MBGL_MAP_MAP
#define MBGL_MAP_MAP

#include <mbgl/util/chrono.hpp>
#include <mbgl/map/update.hpp>
#include <mbgl/map/mode.hpp>
#include <mbgl/util/geo.hpp>
#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/vec.hpp>
#include <mbgl/annotation/annotation.hpp>

#include <cstdint>
#include <string>
#include <functional>
#include <vector>
#include <memory>

namespace mbgl {

class FileSource;
class View;
class MapData;
class MapContext;
class StillImage;
class SpriteImage;
class Transform;
class PointAnnotation;
class ShapeAnnotation;
struct CameraOptions;

namespace util {
template <class T> class Thread;
}

struct EdgeInsets {
    double top = 0;
    double left = 0;
    double bottom = 0;
    double right = 0;
};

class Map : private util::noncopyable {
    friend class View;

public:
    explicit Map(View&, FileSource&,
                 MapMode mode = MapMode::Continuous);
    ~Map();

    // Pauses the render thread. The render thread will stop running but will not be terminated and will not lose state until resumed.
    void pause();
    bool isPaused();

    // Resumes a paused render thread
    void resume();

    // Register a callback that will get called (on the render thread) when all resources have
    // been loaded and a complete render occurs.
    using StillImageCallback = std::function<void(std::exception_ptr, std::unique_ptr<const StillImage>)>;
    void renderStill(StillImageCallback callback);

    // Triggers a synchronous render.
    void renderSync();

    // Notifies the Map thread that the state has changed and an update might be necessary.
    void update(Update update);

    // Styling
    void addClass(const std::string&);
    void removeClass(const std::string&);
    bool hasClass(const std::string&) const;
    void setClasses(const std::vector<std::string>&);
    std::vector<std::string> getClasses() const;

    void setDefaultTransitionDuration(const Duration& = Duration::zero());
    Duration getDefaultTransitionDuration() const;

    void setDefaultTransitionDelay(const Duration& = Duration::zero());
    Duration getDefaultTransitionDelay() const;

    void setStyleURL(const std::string& url);
    void setStyleJSON(const std::string& json, const std::string& base = "");
    std::string getStyleURL() const;
    std::string getStyleJSON() const;

    // Transition
    void cancelTransitions();
    void setGestureInProgress(bool);
    bool isGestureInProgress() const;
    bool isRotating() const;
    bool isScaling() const;
    bool isPanning() const;

    // Camera
    void jumpTo(CameraOptions options);
    void easeTo(CameraOptions options);

    // Position
    void moveBy(double dx, double dy, const Duration& = Duration::zero());
    void setLatLng(LatLng latLng, const Duration& = Duration::zero());
    LatLng getLatLng() const;
    void resetPosition();

    // Scale
    void scaleBy(double ds, double cx = -1, double cy = -1, const Duration& = Duration::zero());
    void setScale(double scale, double cx = -1, double cy = -1, const Duration& = Duration::zero());
    double getScale() const;
    void setZoom(double zoom, const Duration& = Duration::zero());
    double getZoom() const;
    void setLatLngZoom(LatLng latLng, double zoom, const Duration& = Duration::zero());
    CameraOptions cameraForLatLngBounds(LatLngBounds bounds, EdgeInsets padding);
    CameraOptions cameraForLatLngs(std::vector<LatLng> latLngs, EdgeInsets padding);
    void resetZoom();
    double getMinZoom() const;
    double getMaxZoom() const;

    // Rotation
    void rotateBy(double sx, double sy, double ex, double ey, const Duration& = Duration::zero());
    void setBearing(double degrees, const Duration& = Duration::zero());
    void setBearing(double degrees, double cx, double cy);
    double getBearing() const;
    void resetNorth();

    // Pitch
    void setPitch(double pitch, const Duration& = Duration::zero());
    double getPitch() const;

    // Size
    uint16_t getWidth() const;
    uint16_t getHeight() const;

    // Projection
    void getWorldBoundsMeters(ProjectedMeters &sw, ProjectedMeters &ne) const;
    void getWorldBoundsLatLng(LatLng &sw, LatLng &ne) const;
    double getMetersPerPixelAtLatitude(const double lat, const double zoom) const;
    const ProjectedMeters projectedMetersForLatLng(const LatLng latLng) const;
    const LatLng latLngForProjectedMeters(const ProjectedMeters projectedMeters) const;
    const vec2<double> pixelForLatLng(const LatLng latLng) const;
    const LatLng latLngForPixel(const vec2<double> pixel) const;

    // Annotations
    AnnotationID addPointAnnotation(const PointAnnotation&);
    AnnotationIDs addPointAnnotations(const std::vector<PointAnnotation>&);

    AnnotationID addShapeAnnotation(const ShapeAnnotation&);
    AnnotationIDs addShapeAnnotations(const std::vector<ShapeAnnotation>&);

    void removeAnnotation(AnnotationID);
    void removeAnnotations(const AnnotationIDs&);

    AnnotationIDs getPointAnnotationsInBounds(const LatLngBounds&);
    LatLngBounds getBoundsForAnnotations(const AnnotationIDs&);
    double getTopOffsetPixelsForAnnotationSymbol(const std::string&);

    // Sprites
    void setSprite(const std::string&, std::shared_ptr<const SpriteImage>);
    void removeSprite(const std::string&);

    // Memory
    void setSourceTileCacheSize(size_t);
    void onLowMemory();

    // Debug
    void setDebug(bool value);
    void toggleDebug();
    bool getDebug() const;
    void setCollisionDebug(bool value);
    void toggleCollisionDebug();
    bool getCollisionDebug() const;
    bool isFullyLoaded() const;

private:
    View& view;
    const std::unique_ptr<Transform> transform;
    const std::unique_ptr<MapData> data;
    const std::unique_ptr<util::Thread<MapContext>> context;

    enum class RenderState {
        never,
        partial,
        fully
    };

    RenderState renderState = RenderState::never;
};

}

#endif
