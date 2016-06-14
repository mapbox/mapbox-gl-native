#pragma once

#include <mbgl/style/source.hpp>

namespace mbgl {
namespace style {

class RasterSource : public Source {
public:
    RasterSource(std::string id,
                 std::string url,
                 uint16_t tileSize,
                 std::unique_ptr<Tileset>&&);
};

} // namespace style
} // namespace mbgl
