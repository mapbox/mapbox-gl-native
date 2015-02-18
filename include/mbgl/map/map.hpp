#ifndef MBGL_MAP_MAP
#define MBGL_MAP_MAP

#include <mbgl/map/transform.hpp>
#include <mbgl/util/geo.hpp>
#include <mbgl/util/projection.hpp>
#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/uv.hpp>
#include <mbgl/util/ptr.hpp>
#include <mbgl/util/vec.hpp>

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
class StillImage;

class Map : private util::noncopyable {
    friend class View;

public:
    enum class RenderMode : bool {
        Continuous, // continually updating map
        Still, // a one-off still image.
    };

    explicit Map(View&, FileSource&, RenderMode mode = RenderMode::Continuous);
    ~Map();

    // Start the map render thread. It is asynchronous.
    void start();

    // Stop the map render thread. The thread will not be joined, instead the map will cease to
    // render any further things.
    void stop();

    // Renders a still image in the background thread.
    using StillImageCallback = std::function<void(std::unique_ptr<const StillImage>)>;
    void renderStill(StillImageCallback callback);

    // Returns true when the Map object is currently used to render a map.
    bool isRendering() const;

private:
    // Runs the map event loop. ONLY run this function when you want to get render a single frame
    // with this map object. It will *not* spawn a separate thread and instead block until the
    // frame is completely rendered.
    void run();

    void runContinuous();
    void updatedContinuous();

    void runStillImage();
    void updatedStillImage();


public:
    // Forces a map update: always triggers a rerender. Must be called from the UI thread.
    void update();

    // Triggers a rerender from the map thread.
    void rerender();

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
    void setLatLng(LatLng latLng, std::chrono::steady_clock::duration duration = std::chrono::steady_clock::duration::zero());
    const LatLng getLatLng() const;
    void startPanning();
    void stopPanning();
    void resetPosition();

    // Scale
    void scaleBy(double ds, double cx = -1, double cy = -1, std::chrono::steady_clock::duration duration = std::chrono::steady_clock::duration::zero());
    void setScale(double scale, double cx = -1, double cy = -1, std::chrono::steady_clock::duration duration = std::chrono::steady_clock::duration::zero());
    double getScale() const;
    void setZoom(double zoom, std::chrono::steady_clock::duration duration = std::chrono::steady_clock::duration::zero());
    double getZoom() const;
    void setLatLngZoom(LatLng latLng, double zoom, std::chrono::steady_clock::duration duration = std::chrono::steady_clock::duration::zero());
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

    // Projection
    inline void getWorldBoundsMeters(ProjectedMeters &sw, ProjectedMeters &ne) const { Projection::getWorldBoundsMeters(sw, ne); }
    inline void getWorldBoundsLatLng(LatLng &sw, LatLng &ne) const { Projection::getWorldBoundsLatLng(sw, ne); }
    inline double getMetersPerPixelAtLatitude(const double lat, const double zoom) const { return Projection::getMetersPerPixelAtLatitude(lat, zoom); }
    inline const ProjectedMeters projectedMetersForLatLng(const LatLng latLng) const { return Projection::projectedMetersForLatLng(latLng); }
    inline const LatLng latLngForProjectedMeters(const ProjectedMeters projectedMeters) const { return Projection::latLngForProjectedMeters(projectedMeters); }
    inline const vec2<double> pixelForLatLng(const LatLng latLng) const { return state.pixelForLatLng(latLng); }
    inline const LatLng latLngForPixel(const vec2<double> pixel) const { return state.latLngForPixel(pixel); }

    // Debug
    void setDebug(bool value);
    void toggleDebug();
    bool getDebug() const;

    inline const TransformState &getState() const { return state; }
    inline std::chrono::steady_clock::time_point getTime() const { return animationTime; }

private:
    // This may only be called by the View object.
    void resize(uint16_t width, uint16_t height, float ratio, uint16_t fbWidth, uint16_t fbHeight);

    util::ptr<Sprite> getSprite();
    uv::worker& getWorker();

    // Setup
    void setup();

    void loadStyleURL();

    void updateTiles();
    void updateSources();
    void updateSources(const util::ptr<StyleLayerGroup> &group);

    // Prepares a map render by updating the tiles we need for the current view, as well as updating
    // the stylesheet.
    void prepare();


    // Unconditionally performs a render with the current map state.
    void render();

    // Helper functions
    bool inUIThread() const;
    bool inMapThread() const;

    // This is invoked when the Map object is about to be destructed.
    void terminate();

    void updated();

private:
    View &view;
    FileSource& fileSource;
    const RenderMode renderMode;
public: // TODO: Make this private again.
    std::unique_ptr<uv::loop> loop;
private:
    const std::thread::id uiThread;
    std::thread::id mapThread;
    Transform transform;

    std::unique_ptr<uv::async> asyncTerminate;
    std::unique_ptr<uv::async> asyncUpdate;

    std::thread thread;

    std::mutex rendering;

    std::atomic<bool> active;

private:
    std::unique_ptr<uv::worker> workers;

    TransformState state;

    util::ptr<Style> style;
    std::unique_ptr<GlyphAtlas> glyphAtlas;
    util::ptr<GlyphStore> glyphStore;
    std::unique_ptr<SpriteAtlas> spriteAtlas;
    util::ptr<Sprite> sprite;
    std::unique_ptr<LineAtlas> lineAtlas;
    util::ptr<TexturePool> texturePool;

    std::unique_ptr<Painter> painter;

    std::string styleURL;
    std::string styleJSON = "";
    std::vector<std::string> classes;
    std::string accessToken;

    std::string glyphURL;

    std::chrono::steady_clock::duration defaultTransitionDuration;

    bool debug = false;
    std::chrono::steady_clock::time_point animationTime = std::chrono::steady_clock::time_point::min();

    std::set<util::ptr<StyleSource>> activeSources;

    StillImageCallback callback;
};

}

#endif
