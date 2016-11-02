#include <mbgl/style/sources/custom_vector_source.hpp>
#include <mbgl/style/sources/custom_vector_source_impl.hpp>

namespace mbgl {
  namespace style {
    
    CustomVectorSource::CustomVectorSource(std::string id, CustomVectorSourceOptions options, std::function<void(uint8_t, uint32_t, uint32_t)> fetchTile)
    : Source(SourceType::Vector, std::make_unique<CustomVectorSource::Impl>(std::move(id), *this,  options, fetchTile)) {
    }
    
  } // namespace style
} // namespace mbgl
