#pragma once

#include <mbgl/style/source.hpp>
#include <mbgl/util/geojson.hpp>

#include <mapbox/geojson.hpp>

namespace mapbox {

namespace geojsonvt {
class GeoJSONVT;
} // namespace geojsonvt

namespace supercluster {
class Supercluster;
} // namespace supercluster

} // namespace mapbox

namespace mbgl {
namespace style {

using GeoJSONVTPointer = std::unique_ptr<mapbox::geojsonvt::GeoJSONVT>;
using SuperclusterPointer = std::unique_ptr<mapbox::supercluster::Supercluster>;

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
    GeoJSONSource(const std::string& id, const GeoJSONOptions options_ = GeoJSONOptions());

    void setURL(const std::string& url);
    void setGeoJSON(const GeoJSON&);

    std::string getURL();

    // Private implementation

    class Impl;
    Impl* const impl;
};

} // namespace style
} // namespace mbgl
