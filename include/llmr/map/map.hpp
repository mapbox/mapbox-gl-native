#ifndef LLMR_MAP_MAP
#define LLMR_MAP_MAP

#include "../platform/platform.hpp"
#include "../renderer/painter.hpp"
#include "transform.hpp"
#include <llmr/map/tile.hpp>
#include "settings.hpp"
#include "../style/style.hpp"

#include <forward_list>

namespace llmr {

class Map {
public:
    Map(Settings& settings);
    ~Map();


    // Make noncopyable
    Map(const Map&) = delete;
    Map(const Map&&) = delete;
    Map &operator=(const Map&) = delete;
    Map &operator=(const Map&&) = delete;

    void setup();
    void loadStyle(const uint8_t *const data, uint32_t bytes);
    void loadSettings();
    void resize(uint32_t width, uint32_t height, uint32_t fb_width, uint32_t fb_height);

    /* callback */
    bool render();
    void moveBy(double dx, double dy);
    void scaleBy(double ds, double cx, double cy);
    void rotateBy(double cx, double cy, double sx, double sy, double ex, double ey);
    void tileLoaded(Tile::Ptr tile);
    void tileFailed(Tile::Ptr tile);

    /* position */
    void resetNorth();
    void resetPosition();
    // void setAngle(double angle);
    // void setLonLat(double lon, double lat);
    // void setZoom(double zoom);
    // void setLonLatZoom(double lon, double lat, double zoom);

    void toggleDebug();

private:
    bool findLoadedChildren(const Tile::ID& id, int32_t maxCoveringZoom, std::forward_list<Tile::ID>& retain);
    bool findLoadedParent(const Tile::ID& id, int32_t minCoveringZoom, std::forward_list<Tile::ID>& retain);
    void updateTiles();
    Tile::Ptr addTile(const Tile::ID& id);
    Tile::Ptr hasTile(const Tile::ID& id);


    void update();

private:
    Settings& settings;
    Transform transform;
    Style style;
    Painter painter;

    int32_t min_zoom;
    int32_t max_zoom;

    std::forward_list<Tile::Ptr> tiles;
    std::forward_list<Tile::Ptr> historic_tiles;
};

}

#endif
