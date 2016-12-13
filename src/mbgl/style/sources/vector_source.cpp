#include <mbgl/style/sources/vector_source.hpp>
#include <mbgl/style/sources/vector_source_impl.hpp>

namespace mbgl {
namespace style {

VectorSource::VectorSource(std::string id, variant<std::string, Tileset> urlOrTileset)
    : Source(SourceType::Vector, std::make_unique<VectorSource::Impl>(std::move(id), *this, std::move(urlOrTileset))),
      impl(static_cast<Impl*>(baseImpl.get())) {
}

optional<std::string> VectorSource::getURL() const {
    auto urlOrTileset = impl->getURLOrTileset();
    if (urlOrTileset.is<std::string>()) {
        return urlOrTileset.get<std::string>();
    } else {
        return {};
    }
}

} // namespace style
} // namespace mbgl
