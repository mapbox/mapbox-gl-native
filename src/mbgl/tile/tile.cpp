#include <mbgl/tile/tile.hpp>
#include <mbgl/tile/tile_observer.hpp>
#include <mbgl/renderer/debug_bucket.hpp>
#include <mbgl/util/string.hpp>
#include <mbgl/util/logging.hpp>
#include <mbgl/map/query.hpp>

namespace mbgl {

static TileObserver nullObserver;

Tile::Tile(OverscaledTileID id_) : id(std::move(id_)), observer(&nullObserver) {
}

Tile::~Tile() = default;

void Tile::setObserver(TileObserver* observer_) {
    observer = observer_;
}

void Tile::setTriedOptional() {
    triedOptional = true;
    observer->onTileChanged(*this);
}

void Tile::dumpDebugLogs() const {
    Log::Info(Event::General, "Tile::id: %s", util::toString(id).c_str());
    Log::Info(Event::General, "Tile::renderable: %s", isRenderable() ? "yes" : "no");
    Log::Info(Event::General, "Tile::complete: %s", isComplete() ? "yes" : "no");
}

void Tile::queryRenderedFeatures(
        std::unordered_map<std::string, std::vector<Feature>>&,
        const GeometryCoordinates&,
        const TransformState&,
        const RenderedQueryOptions&) {}

void Tile::querySourceFeatures(
        std::vector<Feature>&,
        const SourceQueryOptions&) {}

} // namespace mbgl
