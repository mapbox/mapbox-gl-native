#include <mbgl/style/sources/vector_source.hpp>
#include <mbgl/style/sources/vector_source_impl.hpp>

namespace mbgl {
namespace style {

VectorSource::VectorSource(std::string id, variant<std::string, Tileset> urlOrTileset)
    : Source(SourceType::Vector, std::make_unique<VectorSource::Impl>(std::move(id), *this, std::move(urlOrTileset))) {
}

} // namespace style
} // namespace mbgl
