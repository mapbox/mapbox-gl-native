#ifndef MBGL_MAP_MAP
#define MBGL_MAP_MAP

#include <mbgl/map/transform.hpp>
#include <mbgl/renderer/painter.hpp>

#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/time.hpp>
#include <mbgl/util/uv.hpp>
#include <mbgl/util/ptr.hpp>

#include <cstdint>
#include <atomic>
#include <iosfwd>
#include <set>
#include <vector>

namespace mbgl {

class GlyphAtlas;
class GlyphStore;
class LayerDescription;
class SpriteAtlas;
class Sprite;
class Style;
class StyleLayer;
class StyleLayerGroup;
class StyleSource;
class Texturepool;
class FileSource;
class View;

class Map : private util::noncopyable {
    typedef void (*stop_callback)();

public:
    explicit Map(View &view);
    ~Map();

    // Start/stop the map render thread. the start() call is asynchronous, while the stop() call
    // will block until the map rendering thread stopped.
    void start();
    void stop(stop_callback cb = nullptr);

    // Runs the map event loop. ONLY run this function when you want to get render a single frame
    // with this map object. It will *not* spawn a separate thread and instead block until the
    // frame is completely rendered.
    void run();

    // Triggers a lazy rerender: only performs a render when the map is not clean.
    void rerender();

    void renderLayer(util::ptr<StyleLayer> layer_desc, RenderPass pass, const Tile::ID* id = nullptr, const mat4* matrix = nullptr);

    // Forces a map update: always triggers a rerender.
    void update();

    // Triggers a cleanup that releases resources.
    void cleanup();

    // Controls buffer swapping.
    bool needsSwap();
    void swapped();

    // Size
    void resize(uint16_t width, uint16_t height, float ratio = 1);
    void resize(uint16_t width, uint16_t height, float ratio, uint16_t fb_width, uint16_t fb_height);

    // Styling
    const std::set<util::ptr<StyleSource>> getActiveSources() const;
    void setAppliedClasses(const std::vector<std::string> &classes);
    void toggleClass(const std::string &name);
    const std::vector<std::string> &getAppliedClasses() const;
    void setDefaultTransitionDuration(uint64_t duration_milliseconds = 0);
    void setStyleURL(const std::string &url);
    void setStyleJSON(std::string newStyleJSON, const std::string &base = "");
    std::string getStyleJSON() const;
    void setAccessToken(std::string access_token);
    std::string getAccessToken() const;

    // Transition
    void cancelTransitions();

    // Position
    void moveBy(double dx, double dy, double duration = 0);
    void setLonLat(double lon, double lat, double duration = 0);
    void getLonLat(double &lon, double &lat) const;
    void startPanning();
    void stopPanning();
    void resetPosition();

    // Scale
    void scaleBy(double ds, double cx = -1, double cy = -1, double duration = 0);
    void setScale(double scale, double cx = -1, double cy = -1, double duration = 0);
    double getScale() const;
    void setZoom(double zoom, double duration = 0);
    double getZoom() const;
    void setLonLatZoom(double lon, double lat, double zoom, double duration = 0);
    void getLonLatZoom(double &lon, double &lat, double &zoom) const;
    void resetZoom();
    void startScaling();
    void stopScaling();
    double getMinZoom() const;
    double getMaxZoom() const;

    // Rotation
    void rotateBy(double sx, double sy, double ex, double ey, double duration = 0);
    void setBearing(double degrees, double duration = 0);
    void setBearing(double degrees, double cx, double cy);
    double getBearing() const;
    void resetNorth();
    void startRotating();
    void stopRotating();
    bool canRotate();

    // Debug
    void setDebug(bool value);
    void toggleDebug();
    bool getDebug() const;

public:
    inline const TransformState &getState() const { return state; }
    inline util::ptr<FileSource> getFileSource() const { return fileSource; }
    inline util::ptr<Style> getStyle() const { return style; }
    inline util::ptr<GlyphAtlas> getGlyphAtlas() { return glyphAtlas; }
    inline util::ptr<GlyphStore> getGlyphStore() { return glyphStore; }
    inline util::ptr<SpriteAtlas> getSpriteAtlas() { return spriteAtlas; }
    util::ptr<Sprite> getSprite();
    inline util::ptr<Texturepool> getTexturepool() { return texturepool; }
    inline util::ptr<uv::loop> getLoop() { return loop; }
    uv::worker &getWorker();
    inline timestamp getAnimationTime() const { return animationTime; }
    inline timestamp getTime() const { return animationTime; }
    void updateTiles();

private:
    // uv async callbacks
    static void render(uv_async_t *async);
    static void terminate(uv_async_t *async);
    static void cleanup(uv_async_t *async);
    static void delete_async(uv_handle_t *handle);

    // Setup
    void setup();

    void updateSources();
    void updateSources(const util::ptr<StyleLayerGroup> &group);

    void updateRenderState();

    size_t countLayers(const std::vector<LayerDescription>& layers);

    // Prepares a map render by updating the tiles we need for the current view, as well as updating
    // the stylesheet.
    void prepare();


    // Unconditionally performs a render with the current map state.
    void render();
    void renderLayers(util::ptr<StyleLayerGroup> group);

private:
    bool async = false;
    util::ptr<uv::loop> loop;
    std::unique_ptr<uv::worker> workers;
    std::unique_ptr<uv::thread> thread;
    std::unique_ptr<uv_async_t> async_terminate;
    std::unique_ptr<uv_async_t> async_render;
    std::unique_ptr<uv_async_t> async_cleanup;

private:
    // If cleared, the next time the render thread attempts to render the map, it will *actually*
    // render the map.
    std::atomic_flag is_clean = ATOMIC_FLAG_INIT;

    // If this flag is cleared, the current back buffer is ready for being swapped with the front
    // buffer (i.e. it has rendered data).
    std::atomic_flag is_swapped = ATOMIC_FLAG_INIT;

    // This is cleared once the current front buffer has been presented and the back buffer is
    // ready for rendering.
    std::atomic_flag is_rendered = ATOMIC_FLAG_INIT;

    // Stores whether the map thread has been stopped already.
    std::atomic_bool is_stopped;

public:
    View &view;

private:
#ifndef NDEBUG
    const unsigned long main_thread;
    unsigned long map_thread = -1;
#endif

    Transform transform;
    TransformState state;

    util::ptr<FileSource> fileSource;

    util::ptr<Style> style;
    util::ptr<GlyphAtlas> glyphAtlas;
    util::ptr<GlyphStore> glyphStore;
    util::ptr<SpriteAtlas> spriteAtlas;
    util::ptr<Sprite> sprite;
    util::ptr<Texturepool> texturepool;

    Painter painter;

    std::string styleURL;
    std::string styleJSON = "";
    std::string accessToken = "";

    bool debug = false;
    timestamp animationTime = 0;

    int indent = 0;

    std::set<util::ptr<StyleSource>> activeSources;

};

}

#endif
