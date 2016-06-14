#include <mbgl/style/sources/raster_source.hpp>
#include <mbgl/tile/raster_tile.hpp>
#include <mbgl/platform/log.hpp>

namespace mbgl {
namespace style {

std::unique_ptr<RasterSource> RasterSource::parse(std::string id, const JSValue& value) {
    optional<variant<std::string, Tileset>> urlOrTileset = TileSource::parseURLOrTileset(value);
    if (!urlOrTileset) {
        return nullptr;
    }

    uint16_t tileSize = util::tileSize;
    if (value.HasMember("tileSize")) {
        const JSValue& tileSizeVal = value["tileSize"];
        if (tileSizeVal.IsNumber() && tileSizeVal.GetUint64() <= std::numeric_limits<uint16_t>::max()) {
            tileSize = tileSizeVal.GetUint64();
        } else {
            Log::Error(Event::ParseStyle, "invalid tileSize");
            return nullptr;
        }
    }

    return std::make_unique<RasterSource>(std::move(id), std::move(*urlOrTileset), tileSize);
}

RasterSource::RasterSource(std::string id_,
                           variant<std::string, Tileset> urlOrTileset_,
                           uint16_t tileSize_)
    : TileSource(SourceType::Raster, std::move(id_), std::move(urlOrTileset_), tileSize_) {
}

std::unique_ptr<Tile> RasterSource::createTile(const OverscaledTileID& tileID,
                                               const UpdateParameters& parameters) {
    return std::make_unique<RasterTile>(tileID, parameters, tileset);
}

} // namespace style
} // namespace mbgl
