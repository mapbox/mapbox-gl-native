#ifndef LLMR_MAP_MAP
#define LLMR_MAP_MAP

#include <llmr/map/tile.hpp>
#include <llmr/map/transform.hpp>
#include <llmr/style/style.hpp>
#include <llmr/renderer/painter.hpp>
#include <llmr/util/noncopyable.hpp>

#include <cstdint>
#include <string>

namespace llmr {

class Settings;

class Map : private util::noncopyable {
public:
    explicit Map(Settings& settings);
    ~Map();

    /* setup */
    void setup();
    void loadStyle(const uint8_t *const data, uint32_t bytes);
    void loadSprite(const std::string& url);
    void loadSettings();
    void resize(uint32_t width, uint32_t height, uint32_t fb_width, uint32_t fb_height);

    /* callback */
    bool render();
    void cancelAnimations();
    void tileLoaded(std::shared_ptr<Tile> tile);
    void tileFailed(std::shared_ptr<Tile> tile);

    /* position */
    void moveBy(double dx, double dy, double duration = 0);
    void setLonLat(double lon, double lat, double duration = 0);
    void getLonLat(double &lon, double &lat) const;
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

    /* rotation */
    void rotateBy(double cx, double cy, double sx, double sy, double ex, double ey, double duration = 0);
    void setAngle(double angle, double cx = -1, double cy = -1, double duration = 0);
    double getAngle() const;
    void resetNorth();

    void toggleDebug();

private:
    bool findLoadedChildren(const Tile::ID& id, int32_t maxCoveringZoom, std::forward_list<Tile::ID>& retain);
    bool findLoadedParent(const Tile::ID& id, int32_t minCoveringZoom, std::forward_list<Tile::ID>& retain);
    bool updateTiles();
    std::shared_ptr<Tile> addTile(const Tile::ID& id);
    std::shared_ptr<Tile> hasTile(const Tile::ID& id);


    void update();

private:
    Settings& settings;
    Transform transform;
    Style style;
    Painter painter;

    int32_t min_zoom;
    int32_t max_zoom;

    std::forward_list<std::shared_ptr<Tile>> tiles;
    std::forward_list<std::shared_ptr<Tile>> historic_tiles;
};

}

#endif
