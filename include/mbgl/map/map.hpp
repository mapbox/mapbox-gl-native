#ifndef MBGL_MAP_MAP
#define MBGL_MAP_MAP

#include <mbgl/util/chrono.hpp>
#include <mbgl/map/update.hpp>
#include <mbgl/util/geo.hpp>
#include <mbgl/util/projection.hpp>
#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/ptr.hpp>
#include <mbgl/util/vec.hpp>

#include <cstdint>
#include <atomic>
#include <thread>
#include <iosfwd>
#include <set>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>

namespace uv { class async; }

namespace mbgl {

class GlyphStore;
class Sprite;
class Style;
class TexturePool;
class FileSource;
class View;
class Environment;
class EnvironmentScope;
class MapData;
class Worker;
class StillImage;
class TileID;
class MapContext;

class Map : private util::noncopyable {
    friend class View;

public:
    enum class Mode : uint8_t {
        None, // we're not doing any processing
        Continuous, // continually updating map
        Still, // a once-off still image.
    };

    explicit Map(View&, FileSource&);
    ~Map();

    // Start the map render thread. It is asynchronous.
    void start(bool startPaused = false, Mode mode = Mode::Continuous);
    inline void start(Mode renderMode) { start(false, renderMode); }

    // Stop the map render thread. This call will block until the map rendering thread stopped.
    // The optional callback function will be invoked repeatedly until the map thread is stopped.
    // The callback function should wait until it is woken up again by view.notify(), otherwise
    // this will be a busy waiting loop.
    void stop(std::function<void ()> callback = std::function<void ()>());

    // Pauses the render thread. The render thread will stop running but will not be terminated and will not lose state until resumed.
    void pause(bool waitForPause = false);

    // Resumes a paused render thread
    void resume();

    using StillImageCallback = std::function<void(std::unique_ptr<const StillImage>)>;
    void renderStill(StillImageCallback callback);

    // Triggers a synchronous or asynchronous render.
    void renderSync();

    // Unconditionally performs a render with the current map state. May only be called from the Map
    // thread.
    void render();

    // Notifies the Map thread that the state has changed and an update might be necessary.
    void triggerUpdate(Update = Update::Nothing);

    // Triggers a render. Can be called from any thread.
    void triggerRender();

    // Releases resources immediately
    void terminate();

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
    size_t getSourceTileCacheSize() const { return sourceCacheSize; }
    void onLowMemory();

    // Debug
    void setDebug(bool value);
    void toggleDebug();
    bool getDebug() const;

private:
    // Runs the map event loop. ONLY run this function when you want to get render a single frame
    // with this map object. It will *not* spawn a separate thread and instead block until the
    // frame is completely rendered.
    void run();

    // This may only be called by the View object.
    void resize(uint16_t width, uint16_t height, float ratio = 1);
    void resize(uint16_t width, uint16_t height, float ratio, uint16_t fbWidth, uint16_t fbHeight);

    util::ptr<Sprite> getSprite();
    Worker& getWorker();

    // Checks if render thread needs to pause
    void checkForPause();

    // Setup
    void setup();

    void updateTiles();

    // Triggered by triggerUpdate();
    void update();

    // Loads the style set in the data object. Called by Update::StyleInfo
    void reloadStyle();
    void loadStyleJSON(const std::string& json, const std::string& base);

    // Prepares a map render by updating the tiles we need for the current view, as well as updating
    // the stylesheet.
    void prepare();

    // Runs the function in the map thread.
    void invokeTask(std::function<void()>&&);
    template <typename Fn> auto invokeSyncTask(const Fn& fn) -> decltype(fn());

    void processTasks();

    void updateAnnotationTiles(const std::vector<TileID>&);

    size_t sourceCacheSize;

    Mode mode = Mode::None;

    const std::unique_ptr<Environment> env;
    std::unique_ptr<EnvironmentScope> scope;
    View &view;
    const std::unique_ptr<MapData> data;
    const std::unique_ptr<MapContext> context;

private:
    std::unique_ptr<Worker> workers;
    std::thread thread;
    std::unique_ptr<uv::async> asyncTerminate;
    std::unique_ptr<uv::async> asyncUpdate;
    std::unique_ptr<uv::async> asyncInvoke;
    std::unique_ptr<uv::async> asyncRender;

    bool terminating = false;
    bool pausing = false;
    bool isPaused = false;
    std::mutex mutexRun;
    std::condition_variable condRun;
    std::mutex mutexPause;
    std::condition_variable condPause;

    // Used to signal that rendering completed.
    bool rendered = false;
    std::condition_variable condRendered;
    std::mutex mutexRendered;

    // Stores whether the map thread has been stopped already.
    std::atomic_bool isStopped;

    util::ptr<GlyphStore> glyphStore;
    util::ptr<Style> style;
    util::ptr<Sprite> sprite;
    util::ptr<TexturePool> texturePool;

    std::atomic<UpdateType> updated;

    std::mutex mutexTask;
    std::queue<std::function<void()>> tasks;
    StillImageCallback callback;
};

}

#endif
