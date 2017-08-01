#include <mbgl/tile/tile.hpp>
#include <mbgl/tile/tile_observer.hpp>
#include <mbgl/renderer/buckets/debug_bucket.hpp>
#include <mbgl/renderer/query.hpp>
#include <mbgl/util/string.hpp>
#include <mbgl/util/logging.hpp>

#include <iostream>

namespace mbgl {

static TileObserver nullObserver;

Tile::Tile(OverscaledTileID id_) : id(std::move(id_)), observer(&nullObserver) {
}

Tile::~Tile() = default;

void Tile::setObserver(TileObserver* observer_) {
    observer = observer_;
}

void Tile::dumpDebugLogs() const {
    Log::Info(Event::General, "Tile::id: %s", util::toString(id).c_str());
    Log::Info(Event::General, "Tile::loaded: %s", isLoaded() ? "yes" : "no");
    Log::Info(Event::General, "Tile::renderable: %s", isRenderable() ? "yes" : "no");
}

void Tile::logDebug(const char* name) const {
     // std::cerr << "[" << this << "] " << name << " " << id << " ("
     //           << "loaded: " << loaded << ", "
     //           << "parsed: " << parsed << ", "
     //           << "failed: " << failed << ", "
     //           << "renderable: " << renderable << ", "
     //           << "pending: " << pending << ")" << std::endl;
    (void)name;
}

void Tile::queryRenderedFeatures(
        std::unordered_map<std::string, std::vector<Feature>>&,
        const GeometryCoordinates&,
        const TransformState&,
        const RenderStyle&,
        const RenderedQueryOptions&) {}

void Tile::querySourceFeatures(
        std::vector<Feature>&,
        const SourceQueryOptions&) {}

} // namespace mbgl
