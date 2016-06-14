#include <mbgl/style/tile_source.hpp>

namespace mbgl {
namespace style {

TileSource::TileSource(SourceType type_,
                       std::string id_,
                       std::string url_,
                       uint16_t tileSize_,
                       std::unique_ptr<Tileset>&& tileset_)
    : Source(type_, std::move(id_), std::move(url_), tileSize_, std::move(tileset_)) {
}

} // namespace style
} // namespace mbgl
