#pragma once

#include <mbgl/style/source.hpp>
#include <mbgl/util/geojson.hpp>

namespace mbgl {
namespace style {

class GeoJSONSource : public Source {
public:
    GeoJSONSource(const std::string& id);

    void setURL(const std::string& url);
    void setGeoJSON(GeoJSON&&);

    // Private implementation

    class Impl;
    Impl* const impl;
};

} // namespace style
} // namespace mbgl
