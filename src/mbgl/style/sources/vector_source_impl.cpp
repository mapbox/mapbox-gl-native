#include <mbgl/style/sources/vector_source_impl.hpp>
#include <mbgl/renderer/sources/render_vector_source.hpp>
#include <mbgl/util/constants.hpp>

namespace mbgl {
namespace style {

VectorSource::Impl::Impl(std::string id_, Source& base_, variant<std::string, Tileset> urlOrTileset_)
    : TileSourceImpl(SourceType::Vector, std::move(id_), base_, std::move(urlOrTileset_), util::tileSize) {
}

std::unique_ptr<RenderSource> VectorSource::Impl::createRenderSource() const {
    return std::make_unique<RenderVectorSource>(*this);
}

} // namespace style
} // namespace mbgl
