#include <mbgl/style/sources/vector_source_impl.hpp>
#include <mbgl/renderer/sources/render_vector_source.hpp>

namespace mbgl {
namespace style {

VectorSource::Impl::Impl(std::string id_)
    : Source::Impl(SourceType::Vector, std::move(id_)) {
}

VectorSource::Impl::Impl(const Impl& other, Tileset tileset_)
    : Source::Impl(other),
      tileset(std::move(tileset_)) {
}

optional<Tileset> VectorSource::Impl::getTileset() const {
    return tileset;
}

optional<std::string> VectorSource::Impl::getAttribution() const {
    if (!tileset) {
        return {};
    }
    return tileset->attribution;
}

std::unique_ptr<RenderSource> VectorSource::Impl::createRenderSource() const {
    return std::make_unique<RenderVectorSource>(staticImmutableCast<VectorSource::Impl>(immutableFromThis()));
}

} // namespace style
} // namespace mbgl
