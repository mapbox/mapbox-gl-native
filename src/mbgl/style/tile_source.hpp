#pragma once

#include <mbgl/style/source.hpp>

namespace mbgl {
namespace style {

/*
   Shared implementation for VectorSource and RasterSource. Should eventually
   be refactored to use composition rather than inheritance.
*/
class TileSource : public Source {
public:
    TileSource(SourceType,
               std::string id,
               std::string url,
               uint16_t tileSize,
               std::unique_ptr<Tileset>&&);
};

} // namespace style
} // namespace mbgl
