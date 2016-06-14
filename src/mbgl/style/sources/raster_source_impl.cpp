#include <mbgl/style/sources/raster_source_impl.hpp>
#include <mbgl/tile/raster_tile.hpp>
#include <mbgl/platform/log.hpp>

namespace mbgl {
namespace style {

std::unique_ptr<RasterSource> RasterSource::Impl::parse(std::string id, const JSValue& value) {
    optional<variant<std::string, Tileset>> urlOrTileset = TileSourceImpl::parseURLOrTileset(value);
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

RasterSource::Impl::Impl(std::string id_, Source& base_,
                         variant<std::string, Tileset> urlOrTileset_,
                         uint16_t tileSize_)
    : TileSourceImpl(SourceType::Raster, std::move(id_), base_, std::move(urlOrTileset_), tileSize_) {
}

std::unique_ptr<Tile> RasterSource::Impl::createTile(const OverscaledTileID& tileID,
                                               const UpdateParameters& parameters) {
    return std::make_unique<RasterTile>(tileID, parameters, tileset);
}

} // namespace style
} // namespace mbgl
