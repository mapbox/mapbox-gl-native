#include <mbgl/map/tile_data.hpp>

using namespace mbgl;

TileData::TileData(const TileID& id_)
    : id(id_),
      debugBucket(debugFontBuffer),
      state(State::initial) {
    // Initialize tile debug coordinates
    debugFontBuffer.addText(std::string(id).c_str(), 50, 200, 5);
}
