#include <mbgl/tile/tile.hpp>
#include <mbgl/tile/tile_observer.hpp>
#include <mbgl/renderer/query.hpp>
#include <mbgl/util/string.hpp>
#include <mbgl/util/logging.hpp>

namespace mbgl {

static TileObserver nullObserver;

Tile::Tile(Kind kind_, OverscaledTileID id_) : kind(kind_), id(std::move(id_)), observer(&nullObserver) {
}

Tile::~Tile() = default;

void Tile::setObserver(TileObserver* observer_) {
    observer = observer_;
}

void Tile::cancel() {
}

void Tile::setTriedCache() {
    triedOptional = true;
    observer->onTileChanged(*this);
}

void Tile::dumpDebugLogs() const {
    std::string kindString;
    switch (kind) {
    case Kind::Geometry: kindString = "Geometry"; break;
    case Kind::Raster: kindString = "Raster"; break;
    case Kind::RasterDEM: kindString = "RasterDEM"; break;
    default: kindString = "Unknown"; break;
    }
    Log::Info(Event::General, "Tile::Kind: %s", kindString.c_str());
    Log::Info(Event::General, "Tile::id: %s", util::toString(id).c_str());
    Log::Info(Event::General, "Tile::renderable: %s", isRenderable() ? "yes" : "no");
    Log::Info(Event::General, "Tile::complete: %s", isComplete() ? "yes" : "no");
}

void Tile::queryRenderedFeatures(std::unordered_map<std::string, std::vector<Feature>>&, const GeometryCoordinates&,
                                 const TransformState&, const std::unordered_map<std::string, const RenderLayer*>&,
                                 const RenderedQueryOptions&, const mat4&, const SourceFeatureState&) {}

float Tile::getQueryPadding(const std::unordered_map<std::string, const RenderLayer*>&) {
    return 0;
}

void Tile::querySourceFeatures(
        std::vector<Feature>&,
        const SourceQueryOptions&) {}

} // namespace mbgl
