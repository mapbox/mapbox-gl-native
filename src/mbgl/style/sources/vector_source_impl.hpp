#pragma once

#include <mbgl/style/sources/vector_source.hpp>
#include <mbgl/style/tile_source_impl.hpp>

namespace mbgl {
namespace style {

class VectorSource::Impl : public TileSourceImpl {
public:
    Impl(std::string id, Source&, variant<std::string, Tileset>);

    std::unique_ptr<RenderSource> createRenderSource() const final;
};

} // namespace style
} // namespace mbgl
