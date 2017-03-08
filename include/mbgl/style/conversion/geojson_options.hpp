#pragma once

#include <mbgl/style/conversion.hpp>
#include <mbgl/style/sources/geojson_source.hpp>

namespace mbgl {
namespace style {
namespace conversion {

template <>
struct Converter<GeoJSONOptions> {

    template <class V>
    optional<GeoJSONOptions> operator()(const V& value, Error& error) const {
        GeoJSONOptions options;

        const auto maxzoomValue = objectMember(value, "maxzoom");
        if (maxzoomValue) {
            if (toNumber(*maxzoomValue)) {
                options.maxzoom = static_cast<uint8_t>(*toNumber(*maxzoomValue));
            } else {
                error = { "GeoJSON source maxzoom value must be a number" };
                return {};
            }
        }

        const auto bufferValue = objectMember(value, "buffer");
        if (bufferValue) {
            if (toNumber(*bufferValue)) {
                options.buffer = static_cast<uint16_t>(*toNumber(*bufferValue));
            } else {
                error = { "GeoJSON source buffer value must be a number" };
                return {};
            }
        }

        const auto toleranceValue = objectMember(value, "tolerance");
        if (toleranceValue) {
            if (toNumber(*toleranceValue)) {
                options.tolerance = static_cast<double>(*toNumber(*toleranceValue));
            } else {
                error = { "GeoJSON source tolerance value must be a number" };
                return {};
            }
        }

        const auto clusterValue = objectMember(value, "cluster");
        if (clusterValue) {
            if (toBool(*clusterValue)) {
                options.cluster = *toBool(*clusterValue);
            } else {
                error = { "GeoJSON source cluster value must be a boolean" };
                return {};
            }
        }

        const auto clusterMaxZoomValue = objectMember(value, "clusterMaxZoom");
        if (clusterMaxZoomValue) {
            if (toNumber(*clusterMaxZoomValue)) {
                options.clusterMaxZoom = static_cast<uint8_t>(*toNumber(*clusterMaxZoomValue));
            } else {
                error = { "GeoJSON source clusterMaxZoom value must be a number" };
                return {};
            }
        }

        const auto clusterRadiusValue = objectMember(value, "clusterRadius");
        if (clusterRadiusValue) {
            if (toNumber(*clusterRadiusValue)) {
                options.clusterRadius = static_cast<double>(*toNumber(*clusterRadiusValue));
            } else {
                error = { "GeoJSON source clusterRadius value must be a number" };
                return {};
            }
        }

        return { options };
    }

};

} // namespace conversion
} // namespace style
} // namespace mbgl
