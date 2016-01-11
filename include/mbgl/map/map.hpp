#ifndef MBGL_MAP_MAP
#define MBGL_MAP_MAP

#include <mbgl/util/chrono.hpp>
#include <mbgl/util/image.hpp>
#include <mbgl/map/update.hpp>
#include <mbgl/map/mode.hpp>
#include <mbgl/util/geo.hpp>
#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/vec.hpp>
#include <mbgl/annotation/annotation.hpp>
#include <mbgl/style/types.hpp>

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
class SpriteImage;
class Transform;
class PointAnnotation;
class ShapeAnnotation;
struct CameraOptions;
struct AnimationOptions;

namespace util {
template <class T> class Thread;
} // namespace util

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
                 MapMode mapMode = MapMode::Continuous,
                 GLContextMode contextMode = GLContextMode::Unique,
                 ConstrainMode constrainMode = ConstrainMode::HeightOnly);
    ~Map();

    // Pauses the render thread. The render thread will stop running but will not be terminated and will not lose state until resumed.
    void pause();
    bool isPaused();

    // Resumes a paused render thread
    void resume();

    // Register a callback that will get called (on the render thread) when all resources have
    // been loaded and a complete render occurs.
    using StillImageCallback = std::function<void (std::exception_ptr, PremultipliedImage&&)>;
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

    void setDefaultFadeDuration(const Duration&);
    Duration getDefaultFadeDuration() const;

    void setDefaultTransitionDuration(const Duration&);
    Duration getDefaultTransitionDuration() const;

    void setDefaultTransitionDelay(const Duration&);
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
    void jumpTo(const CameraOptions&);
    void easeTo(const CameraOptions&, const AnimationOptions&);
    void flyTo(const CameraOptions&, const AnimationOptions&);

    // Position
    void moveBy(const PrecisionPoint&, const Duration& = Duration::zero());
    void setLatLng(const LatLng&, const PrecisionPoint&, const Duration& = Duration::zero());
    void setLatLng(const LatLng&, const Duration& = Duration::zero());
    LatLng getLatLng() const;
    void resetPosition();

    // Scale
    void scaleBy(double ds, const PrecisionPoint& = { NAN, NAN }, const Duration& = Duration::zero());
    void setScale(double scale, const PrecisionPoint& = { NAN, NAN }, const Duration& = Duration::zero());
    double getScale() const;
    void setZoom(double zoom, const Duration& = Duration::zero());
    double getZoom() const;
    void setLatLngZoom(const LatLng&, double zoom, const Duration& = Duration::zero());
    CameraOptions cameraForLatLngBounds(const LatLngBounds&, const EdgeInsets&);
    CameraOptions cameraForLatLngs(const std::vector<LatLng>&, const EdgeInsets&);
    void resetZoom();
    double getMinZoom() const;
    double getMaxZoom() const;

    // Rotation
    void rotateBy(const PrecisionPoint& first, const PrecisionPoint& second, const Duration& = Duration::zero());
    void setBearing(double degrees, const Duration& = Duration::zero());
    void setBearing(double degrees, const PrecisionPoint&);
    double getBearing() const;
    void resetNorth(const Duration& = std::chrono::milliseconds(500));

    // Pitch
    void setPitch(double pitch, const Duration& = Duration::zero());
    double getPitch() const;

    // North Orientation
    void setNorthOrientation(NorthOrientation);
    NorthOrientation getNorthOrientation() const;

    // Size
    uint16_t getWidth() const;
    uint16_t getHeight() const;

    // Projection
    MetersBounds getWorldBoundsMeters() const;
    LatLngBounds getWorldBoundsLatLng() const;

    double getMetersPerPixelAtLatitude(double lat, double zoom) const;
    ProjectedMeters projectedMetersForLatLng(const LatLng&) const;
    LatLng latLngForProjectedMeters(const ProjectedMeters&) const;
    PrecisionPoint pixelForLatLng(const LatLng&) const;
    LatLng latLngForPixel(const PrecisionPoint&) const;

    // Annotations
    void addAnnotationIcon(const std::string&, std::shared_ptr<const SpriteImage>);
    void removeAnnotationIcon(const std::string&);
    double getTopOffsetPixelsForAnnotationIcon(const std::string&);

    AnnotationID addPointAnnotation(const PointAnnotation&);
    AnnotationIDs addPointAnnotations(const std::vector<PointAnnotation>&);

    AnnotationID addShapeAnnotation(const ShapeAnnotation&);
    AnnotationIDs addShapeAnnotations(const std::vector<ShapeAnnotation>&);

    void removeAnnotation(AnnotationID);
    void removeAnnotations(const AnnotationIDs&);

    AnnotationIDs getPointAnnotationsInBounds(const LatLngBounds&);
    LatLngBounds getBoundsForAnnotations(const AnnotationIDs&);

    void addCustomLayer(const std::string& id,
                        CustomLayerInitializeFunction,
                        CustomLayerRenderFunction,
                        CustomLayerDeinitializeFunction,
                        void* context,
                        const char* before = nullptr);
    void removeCustomLayer(const std::string& id);

    // Memory
    void setSourceTileCacheSize(size_t);
    void onLowMemory();

    // Debug
    void setDebug(MapDebugOptions);
    void cycleDebugOptions();
    MapDebugOptions getDebug() const;

    bool isFullyLoaded() const;
    void dumpDebugLogs() const;

private:
    View& view;
    const std::unique_ptr<Transform> transform;
    const std::unique_ptr<util::Thread<MapContext>> context;
    MapData* data;

    enum class RenderState {
        never,
        partial,
        fully
    };

    RenderState renderState = RenderState::never;
};

} // namespace mbgl

#endif
