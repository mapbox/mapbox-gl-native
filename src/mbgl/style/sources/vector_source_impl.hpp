#pragma once

#include <mbgl/style/sources/vector_source.hpp>
#include <mbgl/style/tile_source_impl.hpp>

namespace mbgl {
namespace style {

class VectorSource::Impl : public TileSourceImpl {
public:
    Impl(std::string id, Source&, variant<std::string, Tileset>);

private:
    std::unique_ptr<Tile> createTile(const OverscaledTileID&, const UpdateParameters&) final;
};

} // namespace style
} // namespace mbgl
