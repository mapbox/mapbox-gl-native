#include <mbgl/map/tile_data.hpp>
#include <mbgl/renderer/debug_bucket.hpp>

namespace mbgl {

TileData::TileData(const TileID& id_)
    : id(id_),
      state(State::initial) {
}

TileData::~TileData() = default;

const char* TileData::StateToString(const State state) {
    switch (state) {
        case TileData::State::initial: return "initial";
        case TileData::State::invalid : return "invalid";
        case TileData::State::loading : return "loading";
        case TileData::State::loaded : return "loaded";
        case TileData::State::obsolete : return "obsolete";
        case TileData::State::parsed : return "parsed";
        case TileData::State::partial : return "partial";
        default: return "<unknown>";
    }
}

} // namespace mbgl
