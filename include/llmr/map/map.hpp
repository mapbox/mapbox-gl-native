#ifndef LLMR_MAP_MAP
#define LLMR_MAP_MAP

#include "../platform/platform.hpp"
#include "../renderer/painter.hpp"
#include "transform.hpp"
#include <llmr/map/tile.hpp>
#include "settings.hpp"

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
    void loadSettings();
    void resize(uint32_t width, uint32_t height);

    /* callback */
    bool render();
    void moveBy(double dx, double dy);
    void scaleBy(double ds, double cx, double cy);
    void rotateBy(double cx, double cy, double sx, double sy, double ex, double ey);
    void tileLoaded(tile::ptr tile);
    void tileFailed(tile::ptr tile);

    /* position */
    void resetNorth();
    void resetPosition();
    // void setAngle(double angle);
    // void setLonLat(double lon, double lat);
    // void setZoom(double zoom);
    // void setLonLatZoom(double lon, double lat, double zoom);

    void toggleDebug();

private:
    bool findLoadedChildren(const tile_id& id, int32_t maxCoveringZoom, std::forward_list<tile_id>& retain);
    bool findLoadedParent(const tile_id& id, int32_t minCoveringZoom, std::forward_list<tile_id>& retain);
    void updateTiles();
    tile::ptr addTile(const tile_id& id);
    tile::ptr hasTile(const tile_id& id);


    void update();

private:
    Settings& settings;
    Transform transform;
    Painter painter;

    int32_t min_zoom;
    int32_t max_zoom;

    std::forward_list<tile::ptr> tiles;
};

}

#endif
