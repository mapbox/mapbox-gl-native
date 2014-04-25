#ifndef LLMR_MAP_MAP
#define LLMR_MAP_MAP

#include <uv.h>

#include <llmr/map/tile.hpp>
#include <llmr/map/tile_data.hpp>
#include <llmr/map/transform.hpp>
#include <llmr/style/style.hpp>
#include <llmr/geometry/glyph_atlas.hpp>
#include <llmr/renderer/painter.hpp>
#include <llmr/util/noncopyable.hpp>
#include <llmr/util/texturepool.hpp>

#include <cstdint>
#include <string>
#include <map>

namespace llmr {

class Settings;
class Source;

class Map : private util::noncopyable {
public:
    explicit Map(Settings& settings);
    ~Map();

    void start();
    void run();
    void stop();

    /* setup */
    void setup();
    void loadStyle(const uint8_t *const data, uint32_t bytes);
    void loadSettings();
    void resize(uint16_t width, uint16_t height, float ratio = 1);
    void resize(uint16_t width, uint16_t height, float ratio, uint16_t fb_width, uint16_t fb_height);

    /* callback */
    void update();
    void render();
    void cancelAnimations();

    /* position */
    void moveBy(double dx, double dy, double duration = 0);
    void setLonLat(double lon, double lat, double duration = 0);
    void getLonLat(double &lon, double &lat) const;
    void startPanning();
    void stopPanning();
    void resetPosition();

    /* scale */
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

    /* rotation */
    void rotateBy(double sx, double sy, double ex, double ey, double duration = 0);
    void setAngle(double angle, double cx = -1, double cy = -1, double duration = 0);
    double getAngle() const;
    void resetNorth();
    void startRotating();
    void stopRotating();


    void redraw();

    void toggleDebug();
    void toggleRaster();

    box cornersToBox(uint32_t z) const;


public: // Getters
    float getPixelRatio() const;
    Style& getStyle();
    GlyphAtlas& getGlyphAtlas();
    uv_loop_t *getLoop();

private:
    bool updateTiles();
    static void eventloop(void *arg);

public:
    std::atomic_flag clean = ATOMIC_FLAG_INIT;

private:
    Settings& settings;
    Transform transform;
    Texturepool texturepool;
    Style style;
    GlyphAtlas glyphAtlas;
    Painter painter;

    std::map<std::string, const std::unique_ptr<Source>> sources;

private:
    uv_loop_t *loop = nullptr;
    uv_thread_t thread;
    uv_async_t async_terminate;
};

}

#endif
