#ifndef MBGL_MAP_MAP
#define MBGL_MAP_MAP

#include <mbgl/util/chrono.hpp>
#include <mbgl/map/update.hpp>
#include <mbgl/map/mode.hpp>
#include <mbgl/util/geo.hpp>
#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/vec.hpp>

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

namespace util {
template <class T> class Thread;
}

class Map : private util::noncopyable {
    friend class View;

public:
    explicit Map(View&, FileSource&,
                 MapMode mode = MapMode::Continuous,
                 bool startPaused = false);
    ~Map();

    // Pauses the render thread. The render thread will stop running but will not be terminated and will not lose state until resumed.
    void pause();

    // Resumes a paused render thread
    void resume();

    // Register a callback that will get called (on the render thread) when all resources have
    // been loaded and a complete render occurs.
    using StillImageCallback = std::function<void(std::unique_ptr<const StillImage>)>;
    void renderStill(StillImageCallback callback);

    // Triggers a synchronous or asynchronous render.
    void renderSync();
    void renderAsync();

    // Notifies the Map thread that the state has changed and an update might be necessary.
    void update(Update update = Update::Nothing);

    // Styling
    void addClass(const std::string&);
    void removeClass(const std::string&);
    bool hasClass(const std::string&) const;
    void setClasses(const std::vector<std::string>&);
    std::vector<std::string> getClasses() const;

    void setDefaultTransitionDuration(Duration = Duration::zero());
    Duration getDefaultTransitionDuration();
    void setStyleURL(const std::string& url);
    void setStyleJSON(const std::string& json, const std::string& base = "");
    std::string getStyleURL() const;
    std::string getStyleJSON() const;

    // Transition
    void cancelTransitions();
    void setGestureInProgress(bool);

    // Position
    void moveBy(double dx, double dy, Duration = Duration::zero());
    void setLatLng(LatLng latLng, Duration = Duration::zero());
    LatLng getLatLng() const;
    void resetPosition();

    // Scale
    void scaleBy(double ds, double cx = -1, double cy = -1, Duration = Duration::zero());
    void setScale(double scale, double cx = -1, double cy = -1, Duration = Duration::zero());
    double getScale() const;
    void setZoom(double zoom, Duration = Duration::zero());
    double getZoom() const;
    void setLatLngZoom(LatLng latLng, double zoom, Duration = Duration::zero());
    void resetZoom();
    double getMinZoom() const;
    double getMaxZoom() const;

    // Rotation
    void rotateBy(double sx, double sy, double ex, double ey, Duration = Duration::zero());
    void setBearing(double degrees, Duration = Duration::zero());
    void setBearing(double degrees, double cx, double cy);
    double getBearing() const;
    void resetNorth();

    // Size
    void resize(uint16_t width, uint16_t height, float ratio = 1);
    uint16_t getWidth() const;
    uint16_t getHeight() const;

    // API
    void setAccessToken(const std::string &token);
    std::string getAccessToken() const;

    // Projection
    void getWorldBoundsMeters(ProjectedMeters &sw, ProjectedMeters &ne) const;
    void getWorldBoundsLatLng(LatLng &sw, LatLng &ne) const;
    double getMetersPerPixelAtLatitude(const double lat, const double zoom) const;
    const ProjectedMeters projectedMetersForLatLng(const LatLng latLng) const;
    const LatLng latLngForProjectedMeters(const ProjectedMeters projectedMeters) const;
    const vec2<double> pixelForLatLng(const LatLng latLng) const;
    const LatLng latLngForPixel(const vec2<double> pixel) const;

    // Annotations
    void setDefaultPointAnnotationSymbol(const std::string&);
    double getTopOffsetPixelsForAnnotationSymbol(const std::string&);
    uint32_t addPointAnnotation(const LatLng&, const std::string& symbol);
    std::vector<uint32_t> addPointAnnotations(const std::vector<LatLng>&,
                                              const std::vector<std::string>& symbols);
    void removeAnnotation(uint32_t);
    void removeAnnotations(const std::vector<uint32_t>&);
    std::vector<uint32_t> getAnnotationsInBounds(const LatLngBounds&);
    LatLngBounds getBoundsForAnnotations(const std::vector<uint32_t>&);

    // Memory
    void setSourceTileCacheSize(size_t);
    void onLowMemory();

    // Debug
    void setDebug(bool value);
    void toggleDebug();
    bool getDebug() const;
    bool isFullyLoaded() const;

private:
    const std::unique_ptr<MapData> data;
    const std::unique_ptr<util::Thread<MapContext>> context;
};

}

#endif
