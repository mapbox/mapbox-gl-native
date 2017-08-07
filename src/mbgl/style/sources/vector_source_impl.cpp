#include <mbgl/style/sources/vector_source_impl.hpp>

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

} // namespace style
} // namespace mbgl
