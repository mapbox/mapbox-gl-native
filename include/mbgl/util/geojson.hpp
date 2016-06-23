#pragma once

#include <memory>

namespace mapbox {
namespace geojsonvt {
class GeoJSONVT;
} // namespace geojsonvt
} // namespace mapbox

namespace mbgl {

class GeoJSON {
public:
    GeoJSON(std::unique_ptr<mapbox::geojsonvt::GeoJSONVT>);
    GeoJSON(GeoJSON&&);
    ~GeoJSON();

    std::unique_ptr<mapbox::geojsonvt::GeoJSONVT> impl;
};

} // namespace mbgl
