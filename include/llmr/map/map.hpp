#ifndef LLMR_MAP_MAP
#define LLMR_MAP_MAP


#include <uv.h>

#include <llmr/platform/view.hpp>
#include <llmr/map/configuration.hpp>
#include <llmr/map/tile.hpp>
#include <llmr/map/tile_data.hpp>
#include <llmr/map/transform.hpp>
#include <llmr/map/transform_commands.hpp>
#include <llmr/style/style.hpp>
#include <llmr/style/style.hpp>
#include <llmr/geometry/glyph_atlas.hpp>
#include <llmr/renderer/painter.hpp>
#include <llmr/util/noncopyable.hpp>
#include <llmr/util/texturepool.hpp>
#include <llmr/util/loop.hpp>

#include <cstdint>
#include <string>
#include <queue>

namespace llmr {

class Settings;

class Map : private util::noncopyable {
public:
    // -------------------------------------------------------------------------
    // -- MAIN THREAD ----------------------------------------------------------
    // -------------------------------------------------------------------------

    explicit Map(Settings &settings);

    // Starts the map event loop while rendering in a context.
    void start(ViewContext *context);
    void stop();

    // Returns true if a new frame has been successfully rendered and is
    // available for being displayed.
    bool can_swap() const;

    // Tells the map that the buffer has been swapped and that it
    // could start rendering the next frame if necessary
    void swapped();

    // Changes the viewport and framebuffer dimensions.
    void resize(uint16_t width, uint16_t height, uint16_t fb_width, uint16_t fb_height);

    // Position changes
    void moveBy(int16_t dx, int16_t dy, float duration = 0);
    void resetPosition();

    // Zoom changes
    void scaleBy(double ds, int16_t cx = -1, int16_t cy = -1, float duration = 0);

    // Rotation changes
    void rotateBy(int16_t cx, int16_t cy, int16_t sx, int16_t sy, int16_t ex, int16_t ey, float duration = 0);
    void resetNorth();

    // Enable/disable transforming mode.
    void setTransforming(bool value);

    void toggleDebug();

private:
    // Thread safe. Schedules a rerender.
    void rerender();

    // Sends a transform change to the render thread.
    void scheduleTransform(const TransformCommand &cmd);

    // -------------------------------------------------------------------------
    // -- RENDER THREAD --------------------------------------------------------
    // -------------------------------------------------------------------------

private:
    // This is the entry point for the render_thread
    static void loop(void *ptr);

    // This is called when new transform commands were added to the queue.
    static void async_transform_cb(uv_async_t *async, int status);

    // This is invoked when we need to rerender.
    static void async_render_cb(uv_async_t *async, int status);

    // This is called when the map loop should be terminated.
    static void async_terminate_cb(uv_async_t *async, int status);


    void setup();
    void processTransforms();
    void cascadeStyle();
    void render();

    // /* setup */
    // void setup(float pixelRatio = 1);
    // void loadStyle(const uint8_t *const data, uint32_t bytes);
    void loadSettings();
    // void toggleRaster();

    // /* callback */
    // bool render();
    // void cancelAnimations();

    // /* position */
    // void setLonLat(double lon, double lat, double duration = 0);
    // void getLonLat(double &lon, double &lat) const;
    // void startPanning();
    // void stopPanning();

    // /* scale */
    // void setScale(double scale, double cx = -1, double cy = -1, double duration = 0);
    // double getScale() const;
    // void setZoom(double zoom, double duration = 0);
    // double getZoom() const;
    // void setLonLatZoom(double lon, double lat, double zoom, double duration = 0);
    // void getLonLatZoom(double &lon, double &lat, double &zoom) const;
    // void resetZoom();
    // void startScaling();
    // void stopScaling();

    // /* rotation */
    // void setAngle(double angle, double cx = -1, double cy = -1, double duration = 0);
    // double getAngle() const;
    // void startRotating();
    // void stopRotating();

private:
    bool findLoadedChildren(const Tile::ID& id, int32_t maxCoveringZoom, std::forward_list<Tile::ID>& retain);
    bool findLoadedParent(const Tile::ID& id, int32_t minCoveringZoom, std::forward_list<Tile::ID>& retain);
    bool updateTiles();
    TileData::State addTile(const Tile::ID& id);
    TileData::State hasTile(const Tile::ID& id);

    // void update();

private:
    ViewContext *context = nullptr;

    std::atomic<bool> dirty;
    std::atomic<bool> swap;

    // Thread management
    uv_thread_t render_thread;
    uv::loop render_loop;

    std::mutex transform_commands_lock;
    std::queue<TransformCommand> transform_commands;

    uv_async_t async_transform;
    uv_async_t async_render;
    uv_async_t async_terminate;

private:
    Transform transform;
    Settings &settings;
    Texturepool texturepool;
    Style style;
    GlyphAtlas glyphAtlas;
    Painter painter;

    const int32_t min_zoom;
    const int32_t max_zoom;

    // float pixel_ratio;

    std::atomic<uint8_t> debug;

    // This is read-only on the main thread and will be updated by the render
    // thread on all transformation changes.
    std::atomic<Configuration> config;

    bool use_raster = false;

    std::forward_list<Tile> tiles;
    std::forward_list<std::weak_ptr<TileData>> tile_data;
};

}

#endif
