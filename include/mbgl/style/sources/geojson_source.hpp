#pragma once

#include <mbgl/style/source.hpp>
#include <mbgl/util/geojson.hpp>
#include <mbgl/util/optional.hpp>

namespace mbgl {
namespace style {

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
    GeoJSONSource(const std::string& id, const GeoJSONOptions& = {});

    void setURL(const std::string& url);
    void setGeoJSON(const GeoJSON&);

    optional<std::string> getURL() const;

    // Private implementation

    class Impl;
    Impl* const impl;
};

template <>
inline bool Source::is<GeoJSONSource>() const {
    return type == SourceType::GeoJSON;
}

} // namespace style
} // namespace mbgl
