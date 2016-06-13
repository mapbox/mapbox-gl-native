#pragma once

#include <mbgl/tile/tile_id.hpp>
#include <mbgl/tile/tile_observer.hpp>
#include <mbgl/tile/tile.hpp>
#include <mbgl/tile/tile_cache.hpp>
#include <mbgl/style/types.hpp>

#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/mat4.hpp>
#include <mbgl/util/rapidjson.hpp>
#include <mbgl/util/feature.hpp>
#include <mbgl/util/tileset.hpp>

#include <forward_list>
#include <vector>
#include <map>

namespace mapbox {
namespace geojsonvt {
class GeoJSONVT;
} // namespace geojsonvt
} // namespace mapbox

namespace mbgl {

class Painter;
class FileSource;
class AsyncRequest;
class TransformState;
class RenderTile;
struct ClipID;

namespace style {

class Style;
class UpdateParameters;
class QueryParameters;
class SourceObserver;

class Source : public TileObserver, private util::noncopyable {
public:
    Source(SourceType,
           const std::string& id,
           const std::string& url,
           uint16_t tileSize,
           std::unique_ptr<Tileset>&&,
           std::unique_ptr<mapbox::geojsonvt::GeoJSONVT>&&);
    ~Source();

    bool loaded = false;
    void load(FileSource&);
    bool isLoading() const;
    bool isLoaded() const;

    const Tileset* getTileset() const { return tileset.get(); }

    // Request or parse all the tiles relevant for the "TransformState". This method
    // will return true if all the tiles were scheduled for updating of false if
    // they were not. shouldReparsePartialTiles must be set to "true" if there is
    // new data available that a tile in the "partial" state might be interested at.
    bool update(const UpdateParameters&);

    template <typename ClipIDGenerator>
    void updateClipIDs(ClipIDGenerator& generator) {
        generator.update(renderTiles);
    }

    void updateMatrices(const mat4 &projMatrix, const TransformState &transform);
    void finishRender(Painter &painter);

    const std::map<UnwrappedTileID, RenderTile>& getRenderTiles() const;

    Tile* getTile(const OverscaledTileID&) const;

    std::unordered_map<std::string, std::vector<Feature>>
    queryRenderedFeatures(const QueryParameters&) const;

    void setCacheSize(size_t);
    void onLowMemory();

    void setObserver(SourceObserver* observer);
    void dumpDebugLogs() const;

    const SourceType type;
    const std::string id;
    const std::string url;
    uint16_t tileSize = util::tileSize;
    bool enabled = false;

private:
    // TileObserver implementation.
    void onTileLoaded(Tile&, bool isNewTile) override;
    void onTileError(Tile&, std::exception_ptr) override;
    void onNeedsRepaint() override;

    std::unique_ptr<Tile> createTile(const OverscaledTileID&, const UpdateParameters&);

private:
    std::unique_ptr<const Tileset> tileset;

    std::unique_ptr<mapbox::geojsonvt::GeoJSONVT> geojsonvt;

    // Stores the time when this source was most recently updated.
    TimePoint updated = TimePoint::min();

    std::map<OverscaledTileID, std::unique_ptr<Tile>> tiles;
    std::map<UnwrappedTileID, RenderTile> renderTiles;
    TileCache cache;

    std::unique_ptr<AsyncRequest> req;

    SourceObserver* observer = nullptr;
};

} // namespace style
} // namespace mbgl
