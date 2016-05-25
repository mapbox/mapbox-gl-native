#pragma once

#include <mbgl/tile/tile_id.hpp>
#include <mbgl/tile/tile_data.hpp>
#include <mbgl/tile/tile_cache.hpp>
#include <mbgl/source/source_info.hpp>
#include <mbgl/renderer/renderable.hpp>

#include <mbgl/util/mat4.hpp>
#include <mbgl/util/rapidjson.hpp>
#include <mbgl/util/feature.hpp>

#include <forward_list>
#include <vector>
#include <map>

namespace mapbox {
namespace geojsonvt {
class GeoJSONVT;
} // namespace geojsonvt
} // namespace mapbox

namespace mbgl {

class Style;
class StyleUpdateParameters;
class StyleQueryParameters;
class Painter;
class FileSource;
class AsyncRequest;
class TransformState;
class Tile;
struct ClipID;
class SourceObserver;

class Source : private util::noncopyable {
public:
    Source(SourceType,
           const std::string& id,
           const std::string& url,
           uint16_t tileSize,
           std::unique_ptr<SourceInfo>&&,
           std::unique_ptr<mapbox::geojsonvt::GeoJSONVT>&&);
    ~Source();

    bool loaded = false;
    void load(FileSource&);
    bool isLoading() const;
    bool isLoaded() const;

    const SourceInfo* getInfo() const { return info.get(); }

    // Request or parse all the tiles relevant for the "TransformState". This method
    // will return true if all the tiles were scheduled for updating of false if
    // they were not. shouldReparsePartialTiles must be set to "true" if there is
    // new data available that a tile in the "partial" state might be interested at.
    bool update(const StyleUpdateParameters&);

    template <typename ClipIDGenerator>
    void updateClipIDs(ClipIDGenerator& generator) {
        generator.update(tiles);
    }

    void updateMatrices(const mat4 &projMatrix, const TransformState &transform);
    void finishRender(Painter &painter);

    const std::map<UnwrappedTileID, Tile>& getTiles() const;

    TileData* getTileData(const OverscaledTileID&) const;

    std::unordered_map<std::string, std::vector<Feature>>
    queryRenderedFeatures(const StyleQueryParameters&) const;

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
    void tileLoadingCallback(const OverscaledTileID&, std::exception_ptr, bool isNewTile);

    std::unique_ptr<TileData> createTile(const OverscaledTileID&,
                                         const StyleUpdateParameters& parameters);

private:
    std::unique_ptr<const SourceInfo> info;

    std::unique_ptr<mapbox::geojsonvt::GeoJSONVT> geojsonvt;

    // Stores the time when this source was most recently updated.
    TimePoint updated = TimePoint::min();

    std::map<UnwrappedTileID, Tile> tiles;
    std::map<OverscaledTileID, std::unique_ptr<TileData>> tileDataMap;
    TileCache cache;

    std::unique_ptr<AsyncRequest> req;

    SourceObserver* observer = nullptr;
};

} // namespace mbgl
