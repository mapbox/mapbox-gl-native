#pragma once

#include <mbgl/style/sources/custom_geometry_source.hpp>
#include <mbgl/tile/tile_id.hpp>
#include <mbgl/util/geojson.hpp>
#include <mbgl/actor/actor_ref.hpp>

#include <map>

namespace mbgl {

class CustomGeometryTile;

namespace style {

class CustomTileLoader : private util::noncopyable {
public:

    using OverscaledIDFunctionTuple = std::tuple<uint8_t, int16_t, ActorRef<CustomGeometryTile>>;

    CustomTileLoader(const TileFunction& fetchTileFn, const TileFunction& cancelTileFn);

    void fetchTile(const OverscaledTileID& tileID, ActorRef<CustomGeometryTile> tileRef);
    void cancelTile(const OverscaledTileID& tileID);

    void removeTile(const OverscaledTileID& tileID);
    void setTileData(const CanonicalTileID& tileID, const GeoJSON& data);

    void invalidateTile(const CanonicalTileID&);
    void invalidateRegion(const LatLngBounds&, Range<uint8_t>);

private:
    void invokeTileFetch(const CanonicalTileID& tileID);
    void invokeTileCancel(const CanonicalTileID& tileID);

    TileFunction fetchTileFunction;
    TileFunction cancelTileFunction;
    std::unordered_map<CanonicalTileID, std::vector<OverscaledIDFunctionTuple>> tileCallbackMap;
    // Keep around a cache of tile data to serve back for wrapped and over-zooomed tiles
    std::map<CanonicalTileID, std::unique_ptr<GeoJSON>> dataCache;

};

} // namespace style
} // namespace mbgl
