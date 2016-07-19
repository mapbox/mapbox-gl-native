#pragma once

#include <mbgl/style/source.hpp>

namespace mapbox {

namespace geojsonvt {
class GeoJSONVT;
} // namespace geojsonvt

namespace geojson {
class geojson;
} // namespace geojson

} // namespace mapbox

namespace mbgl {
namespace style {

using GeoJSONVTPointer = std::unique_ptr<mapbox::geojsonvt::GeoJSONVT>;

struct GeoJSONOptions {
    // GeoJSON-VT options
    uint8_t maxzoom = 18;
    uint16_t buffer = 128;
    double tolerance = 0.375;

    // Supercluster options
    bool cluster = false;
    uint16_t clusterRadius = 50;
    uint8_t clusterMaxZoom = 17;
};

class GeoJSONSource : public Source {
public:
    GeoJSONSource(const std::string& id, const GeoJSONOptions& options_ = GeoJSONOptions());

    void setURL(const std::string& url);
    void setGeoJSON(const mapbox::geojson::geojson&);

    // Private implementation

    class Impl;
    Impl* const impl;
};

} // namespace style
} // namespace mbgl
