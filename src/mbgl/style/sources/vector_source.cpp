#include <mbgl/style/sources/vector_source.hpp>

namespace mbgl {
namespace style {

VectorSource::VectorSource(std::string id_,
                           std::string url_,
                           std::unique_ptr<Tileset>&& tileset_)
    : Source(SourceType::Vector, std::move(id_), std::move(url_), util::tileSize, std::move(tileset_)) {
}

} // namespace style
} // namespace mbgl
