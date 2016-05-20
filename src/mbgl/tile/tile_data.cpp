#include <mbgl/tile/tile_data.hpp>
#include <mbgl/tile/tile_source.hpp>
#include <mbgl/renderer/debug_bucket.hpp>
#include <mbgl/util/string.hpp>

namespace mbgl {

TileData::TileData(const OverscaledTileID& id_)
    : id(id_) {
}

TileData::~TileData() = default;

void TileData::setTileSource(std::unique_ptr<TileSource> tileSource_) {
    assert(!tileSource);
    assert(tileSource_);
    tileSource = std::move(tileSource_);
}

void TileData::dumpDebugLogs() const {
    Log::Info(Event::General, "TileData::id: %s", util::toString(id).c_str());
    Log::Info(Event::General, "TileData::renderable: %s", isRenderable() ? "yes" : "no");
    Log::Info(Event::General, "TileData::complete: %s", isComplete() ? "yes" : "no");
}

void TileData::queryRenderedFeatures(
        std::unordered_map<std::string, std::vector<Feature>>&,
        const GeometryCoordinates&,
        const TransformState&,
        const optional<std::vector<std::string>>&) {}

} // namespace mbgl
