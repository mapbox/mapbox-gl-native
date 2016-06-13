#pragma once

#include <mbgl/style/source.hpp>

namespace mbgl {
namespace style {

class GeoJSONSource : public Source {
public:
    GeoJSONSource(std::string id,
                  std::string url,
                  uint16_t tileSize,
                  std::unique_ptr<Tileset>&&,
                  std::unique_ptr<mapbox::geojsonvt::GeoJSONVT>&&);
};

} // namespace style
} // namespace mbgl
