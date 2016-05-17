#include <mbgl/tile/tile_data.hpp>
#include <mbgl/renderer/debug_bucket.hpp>
#include <mbgl/util/string.hpp>

namespace mbgl {

TileData::TileData(const OverscaledTileID& id_)
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

void TileData::dumpDebugLogs() const {
    Log::Info(Event::General, "TileData::id: %s", util::toString(id).c_str());
    Log::Info(Event::General, "TileData::state: %s", TileData::StateToString(state));
}

void TileData::queryRenderedFeatures(
        std::unordered_map<std::string, std::vector<Feature>>&,
        const GeometryCoordinates&,
        const TransformState&,
        const optional<std::vector<std::string>>&) {}

} // namespace mbgl
