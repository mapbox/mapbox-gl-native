
#ifndef LLMR_MAP_MAP
#define LLMR_MAP_MAP

#include <uv.h>

#include <llmr/map/view.hpp>
#include <llmr/map/transform.hpp>
#include <llmr/style/style.hpp>
#include <llmr/geometry/glyph_atlas.hpp>
#include <llmr/text/glyph_store.hpp>
#include <llmr/renderer/painter.hpp>
#include <llmr/util/noncopyable.hpp>
#include <llmr/util/texturepool.hpp>

#include <cstdint>
#include <string>
#include <map>

namespace llmr {

class Source;

class Map : private util::noncopyable {
public:
    explicit Map(View &view);
    ~Map();

    // Start/stop the map render thread
    void start();
    void stop();

    // Runs the map event loop.
    void run();

    // Triggers a lazy rerender: only performs a render when the map is not clean.
    void rerender();

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
    void toggleClass(const std::string &name);
    const std::vector<std::string> &getAppliedClasses() const;
    void setDefaultTransitionDuration(uint64_t duration_milliseconds = 0);
    void setStyleJSON(std::string newStyleJSON);
    std::string getStyleJSON() const;

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
    void setAngle(double angle, double duration = 0);
    void setAngle(double angle, double cx, double cy);
    double getAngle() const;
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
    inline std::shared_ptr<const Style> getStyle() const { return style; }
    inline std::shared_ptr<GlyphAtlas> getGlyphAtlas() { return glyphAtlas; }
    inline std::shared_ptr<GlyphStore> getGlyphStore() { return glyphStore; }
    inline std::shared_ptr<SpriteAtlas> getSpriteAtlas() { return spriteAtlas; }
    inline std::shared_ptr<Texturepool> getTexturepool() { return texturepool; }
    inline std::shared_ptr<uv::loop> getLoop() { return loop; }
    inline time getAnimationTime() const { return animationTime; }
    inline time getTime() const { return animationTime; }

private:
    // uv async callbacks
    static void render(uv_async_t *async);
    static void terminate(uv_async_t *async);
    static void cleanup(uv_async_t *async);
    static void delete_async(uv_handle_t *handle);

    // Setup
    void setup();

    void updateTiles();
    void updateRenderState();

    size_t countLayers(const std::vector<LayerDescription>& layers);

    // Prepares a map render by updating the tiles we need for the current view, as well as updating
    // the stylesheet.
    void prepare();

    enum RenderPass { Opaque, Translucent };

    // Unconditionally performs a render with the current map state.
    void render();
    void renderLayers(std::shared_ptr<StyleLayerGroup> layers);
    void renderLayer(std::shared_ptr<StyleLayer> layer_desc, RenderPass pass);

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

private:
    View &view;
    Transform transform;
    TransformState state;

    std::shared_ptr<Style> style;
    std::shared_ptr<GlyphAtlas> glyphAtlas;
    std::shared_ptr<GlyphStore> glyphStore;
    std::shared_ptr<SpriteAtlas> spriteAtlas;
    std::shared_ptr<Texturepool> texturepool;

    Painter painter;

    std::string styleJSON = "";

    bool debug = false;
    time animationTime = 0;

    int indent = 0;

private:
    bool async = false;
    std::shared_ptr<uv::loop> loop;
    uv_thread_t thread;
    uv_async_t *async_terminate = nullptr;
    uv_async_t *async_render = nullptr;
    uv_async_t *async_cleanup = nullptr;
};

}

#endif
