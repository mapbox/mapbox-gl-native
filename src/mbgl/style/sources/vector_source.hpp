#pragma once

#include <mbgl/style/tile_source.hpp>

namespace mbgl {
namespace style {

class VectorSource : public TileSource {
public:
    static std::unique_ptr<VectorSource> parse(std::string id, const JSValue&);

    VectorSource(std::string id, variant<std::string, Tileset>);

private:
    std::unique_ptr<Tile> createTile(const OverscaledTileID&, const UpdateParameters&) final;
};

} // namespace style
} // namespace mbgl
