#pragma once

#include <mbgl/style/conversion.hpp>
#include <mbgl/style/sources/geojson_source.hpp>

namespace mbgl {
namespace style {
namespace conversion {

template <>
struct Converter<GeoJSONOptions> {

    template <class V>
    Result<GeoJSONOptions> operator()(const V& value) const {
        GeoJSONOptions options;

        const auto maxzoomValue = objectMember(value, "maxzoom");
        if (maxzoomValue) {
            if (toNumber(*maxzoomValue)) {
                options.maxzoom = static_cast<uint8_t>(*toNumber(*maxzoomValue));
            } else {
                return Error{ "GeoJSON source maxzoom value must be a number" };
            }
        }

        const auto bufferValue = objectMember(value, "buffer");
        if (bufferValue) {
            if (toNumber(*bufferValue)) {
                options.buffer = static_cast<uint16_t>(*toNumber(*bufferValue));
            } else {
                return Error{ "GeoJSON source buffer value must be a number" };
            }
        }

        const auto toleranceValue = objectMember(value, "tolerance");
        if (toleranceValue) {
            if (toNumber(*toleranceValue)) {
                options.tolerance = static_cast<double>(*toNumber(*toleranceValue));
            } else {
                return Error{ "GeoJSON source tolerance value must be a number" };
            }
        }

        const auto clusterValue = objectMember(value, "cluster");
        if (clusterValue) {
            if (toBool(*clusterValue)) {
                options.cluster = *toBool(*clusterValue);
            } else {
                return Error{ "GeoJSON source cluster value must be a boolean" };
            }
        }

        const auto clusterMaxZoomValue = objectMember(value, "clusterMaxZoom");
        if (clusterMaxZoomValue) {
            if (toNumber(*clusterMaxZoomValue)) {
                options.clusterMaxZoom = static_cast<uint8_t>(*toNumber(*clusterMaxZoomValue));
            } else {
                return Error{ "GeoJSON source clusterMaxZoom value must be a number" };
            }
        }

        const auto clusterRadiusValue = objectMember(value, "clusterRadius");
        if (clusterRadiusValue) {
            if (toNumber(*clusterRadiusValue)) {
                options.clusterRadius = static_cast<double>(*toNumber(*clusterRadiusValue));
            } else {
                return Error{ "GeoJSON source clusterRadius value must be a number" };
            }
        }

        return { options };
    }

};

} // namespace conversion
} // namespace style
} // namespace mbgl
