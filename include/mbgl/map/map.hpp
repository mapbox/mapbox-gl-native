#ifndef MBGL_MAP_MAP
#define MBGL_MAP_MAP

#include <mbgl/map/transform.hpp>
#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/uv.hpp>
#include <mbgl/util/ptr.hpp>

#include <cstdint>
#include <atomic>
#include <thread>
#include <iosfwd>
#include <set>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <chrono>

namespace mbgl {

class Painter;
class GlyphStore;
class LayerDescription;
class Sprite;
class Style;
class StyleLayer;
class StyleLayerGroup;
class StyleSource;
class TexturePool;
class FileSource;
class View;
class GlyphAtlas;
class SpriteAtlas;
class LineAtlas;

class Map : private util::noncopyable {
public:
    explicit Map(View&, FileSource&);
    ~Map();

    // Start the map render thread. It is asynchronous.
    void start(bool startPaused = false);

    // Stop the map render thread. This call will block until the map rendering thread stopped.
    // The optional callback function will be invoked repeatedly until the map thread is stopped.
    // The callback function should wait until it is woken up again by view.notify(), otherwise
    // this will be a busy waiting loop.
    void stop(std::function<void ()> callback = std::function<void ()>());

    // Pauses the render thread. The render thread will stop running but will not be terminated and will not lose state until resumed.
    void pause(bool waitForPause = false);

    // Resumes a paused render thread
    void resume();

    // Runs the map event loop. ONLY run this function when you want to get render a single frame
    // with this map object. It will *not* spawn a separate thread and instead block until the
    // frame is completely rendered.
    void run();

    // Triggers a lazy rerender: only performs a render when the map is not clean.
    void rerender();

    // Forces a map update: always triggers a rerender.
    void update();

    // Releases resources immediately
    void terminate();

    // Controls buffer swapping.
    bool needsSwap();
    void swapped();

    // Size
    void resize(uint16_t width, uint16_t height, float ratio = 1);
    void resize(uint16_t width, uint16_t height, float ratio, uint16_t fbWidth, uint16_t fbHeight);

    // Styling
    void addClass(const std::string&);
    void removeClass(const std::string&);
    bool hasClass(const std::string&) const;
    void setClasses(const std::vector<std::string>&);
    std::vector<std::string> getClasses() const;

    void setDefaultTransitionDuration(std::chrono::steady_clock::duration duration = std::chrono::steady_clock::duration::zero());
    std::chrono::steady_clock::duration getDefaultTransitionDuration();
    void setStyleURL(const std::string &url);
    void setStyleJSON(std::string newStyleJSON, const std::string &base = "");
    std::string getStyleJSON() const;

    // Transition
    void cancelTransitions();

    // Position
    void moveBy(double dx, double dy, std::chrono::steady_clock::duration duration = std::chrono::steady_clock::duration::zero());
    void setLonLat(double lon, double lat, std::chrono::steady_clock::duration duration = std::chrono::steady_clock::duration::zero());
    void getLonLat(double &lon, double &lat) const;
    void startPanning();
    void stopPanning();
    void resetPosition();

    // Scale
    void scaleBy(double ds, double cx = -1, double cy = -1, std::chrono::steady_clock::duration duration = std::chrono::steady_clock::duration::zero());
    void setScale(double scale, double cx = -1, double cy = -1, std::chrono::steady_clock::duration duration = std::chrono::steady_clock::duration::zero());
    double getScale() const;
    void setZoom(double zoom, std::chrono::steady_clock::duration duration = std::chrono::steady_clock::duration::zero());
    double getZoom() const;
    void setLonLatZoom(double lon, double lat, double zoom, std::chrono::steady_clock::duration duration = std::chrono::steady_clock::duration::zero());
    void getLonLatZoom(double &lon, double &lat, double &zoom) const;
    void resetZoom();
    void startScaling();
    void stopScaling();
    double getMinZoom() const;
    double getMaxZoom() const;

    // Rotation
    void rotateBy(double sx, double sy, double ex, double ey, std::chrono::steady_clock::duration duration = std::chrono::steady_clock::duration::zero());
    void setBearing(double degrees, std::chrono::steady_clock::duration duration = std::chrono::steady_clock::duration::zero());
    void setBearing(double degrees, double cx, double cy);
    double getBearing() const;
    void resetNorth();
    void startRotating();
    void stopRotating();

    // API
    void setAccessToken(const std::string &token);
    const std::string &getAccessToken() const;

    // Debug
    void setDebug(bool value);
    void toggleDebug();
    bool getDebug() const;

    inline const TransformState &getState() const { return state; }
    inline std::chrono::steady_clock::time_point getTime() const { return animationTime; }

private:
    util::ptr<Sprite> getSprite();
    uv::worker& getWorker();

    // Checks if render thread needs to pause
    void checkForPause();

    // Setup
    void setup();

    void updateTiles();
    void updateSources();
    void updateSources(const util::ptr<StyleLayerGroup> &group);

    // Prepares a map render by updating the tiles we need for the current view, as well as updating
    // the stylesheet.
    void prepare();

    // Unconditionally performs a render with the current map state.
    void render();

    enum class Mode : uint8_t {
        None, // we're not doing any processing
        Continuous, // continually updating map
        Static, // a once-off static image.
    };

    Mode mode = Mode::None;

public: // TODO: make private again
    std::unique_ptr<uv::loop> loop;

private:
    std::unique_ptr<uv::worker> workers;
    std::thread thread;
    std::unique_ptr<uv::async> asyncTerminate;
    std::unique_ptr<uv::async> asyncRender;

    bool terminating = false;
    bool pausing = false;
    bool isPaused = false;
    std::mutex mutexRun;
    std::condition_variable condRun;
    std::mutex mutexPause;
    std::condition_variable condPause;

    // If cleared, the next time the render thread attempts to render the map, it will *actually*
    // render the map.
    std::atomic_flag isClean = ATOMIC_FLAG_INIT;

    // If this flag is cleared, the current back buffer is ready for being swapped with the front
    // buffer (i.e. it has rendered data).
    std::atomic_flag isSwapped = ATOMIC_FLAG_INIT;

    // This is cleared once the current front buffer has been presented and the back buffer is
    // ready for rendering.
    std::atomic_flag isRendered = ATOMIC_FLAG_INIT;

    // Stores whether the map thread has been stopped already.
    std::atomic_bool isStopped;

    View &view;

#ifndef NDEBUG
    const std::thread::id mainThread;
    std::thread::id mapThread;
#endif

    Transform transform;
    TransformState state;

    FileSource& fileSource;

    util::ptr<Style> style;
    const std::unique_ptr<GlyphAtlas> glyphAtlas;
    util::ptr<GlyphStore> glyphStore;
    const std::unique_ptr<SpriteAtlas> spriteAtlas;
    util::ptr<Sprite> sprite;
    const std::unique_ptr<LineAtlas> lineAtlas;
    util::ptr<TexturePool> texturePool;

    const std::unique_ptr<Painter> painter;

    std::string styleURL;
    std::string styleJSON = "";
    std::vector<std::string> classes;
    std::string accessToken;

    std::chrono::steady_clock::duration defaultTransitionDuration;

    bool debug = false;
    std::chrono::steady_clock::time_point animationTime = std::chrono::steady_clock::time_point::min();

    std::set<util::ptr<StyleSource>> activeSources;
};

}

#endif
