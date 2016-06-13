#pragma once

#include <mbgl/style/source.hpp>

namespace mbgl {
namespace style {

class VectorSource : public Source {
public:
    VectorSource(std::string id,
                 std::string url,
                 std::unique_ptr<Tileset>&&);

private:
    std::unique_ptr<Tile> createTile(const OverscaledTileID&, const UpdateParameters&) final;
};

} // namespace style
} // namespace mbgl
