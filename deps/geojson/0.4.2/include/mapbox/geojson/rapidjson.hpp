#pragma once

#include <rapidjson/document.h>
#include <mapbox/geojson.hpp>

namespace mapbox {
namespace geojson {

// Use the CrtAllocator, because the MemoryPoolAllocator is broken on ARM
// https://github.com/miloyip/rapidjson/issues/200, 301, 388
using rapidjson_allocator = rapidjson::CrtAllocator;
using rapidjson_document = rapidjson::GenericDocument<rapidjson::UTF8<>, rapidjson_allocator>;
using rapidjson_value = rapidjson::GenericValue<rapidjson::UTF8<>, rapidjson_allocator>;

// Convert inputs of known types. Instantiations are provided for geometry, feature, and
// feature_collection.
template <typename T>
T convert(const rapidjson_value &);

// Convert any GeoJSON type.
geojson convert(const rapidjson_value &);

// Convert back to rapidjson value. Instantiations are provided for geometry, feature, and
// feature_collection.
template <typename T>
rapidjson_value convert(const T &, rapidjson_allocator&);

// Convert any GeoJSON type.
rapidjson_value convert(const geojson &, rapidjson_allocator&);

} // namespace geojson
} // namespace mapbox
