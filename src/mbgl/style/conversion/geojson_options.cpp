#include <mbgl/style/conversion/geojson_options.hpp>
#include <mbgl/style/conversion_impl.hpp>

namespace mbgl {
namespace style {
namespace conversion {

optional<GeoJSONOptions> Converter<GeoJSONOptions>::operator()(const Convertible& value, Error& error) const {
    GeoJSONOptions options;

    const auto minzoomValue = objectMember(value, "minzoom");
    if (minzoomValue) {
        if (toNumber(*minzoomValue)) {
            options.minzoom = static_cast<uint8_t>(*toNumber(*minzoomValue));
        } else {
            error.message = "GeoJSON source minzoom value must be a number";
            return nullopt;
        }
    }

    const auto maxzoomValue = objectMember(value, "maxzoom");
    if (maxzoomValue) {
        if (toNumber(*maxzoomValue)) {
            options.maxzoom = static_cast<uint8_t>(*toNumber(*maxzoomValue));
        } else {
            error.message = "GeoJSON source maxzoom value must be a number";
            return nullopt;
        }
    }

    const auto bufferValue = objectMember(value, "buffer");
    if (bufferValue) {
        if (toNumber(*bufferValue)) {
            options.buffer = static_cast<uint16_t>(*toNumber(*bufferValue));
        } else {
            error.message = "GeoJSON source buffer value must be a number";
            return nullopt;
        }
    }

    const auto toleranceValue = objectMember(value, "tolerance");
    if (toleranceValue) {
        if (toNumber(*toleranceValue)) {
            options.tolerance = static_cast<double>(*toNumber(*toleranceValue));
        } else {
            error.message = "GeoJSON source tolerance value must be a number";
            return nullopt;
        }
    }

    const auto clusterValue = objectMember(value, "cluster");
    if (clusterValue) {
        if (toBool(*clusterValue)) {
            options.cluster = *toBool(*clusterValue);
        } else {
            error.message = "GeoJSON source cluster value must be a boolean";
            return nullopt;
        }
    }

    const auto clusterMaxZoomValue = objectMember(value, "clusterMaxZoom");
    if (clusterMaxZoomValue) {
        if (toNumber(*clusterMaxZoomValue)) {
            options.clusterMaxZoom = static_cast<uint8_t>(*toNumber(*clusterMaxZoomValue));
        } else {
            error.message = "GeoJSON source clusterMaxZoom value must be a number";
            return nullopt;
        }
    }

    const auto clusterRadiusValue = objectMember(value, "clusterRadius");
    if (clusterRadiusValue) {
        if (toNumber(*clusterRadiusValue)) {
            options.clusterRadius = static_cast<double>(*toNumber(*clusterRadiusValue));
        } else {
            error.message = "GeoJSON source clusterRadius value must be a number";
            return nullopt;
        }
    }

    const auto lineMetricsValue = objectMember(value, "lineMetrics");
    if (lineMetricsValue) {
        if (toBool(*lineMetricsValue)) {
            options.lineMetrics = *toBool(*lineMetricsValue);
        } else {
            error = { "GeoJSON source lineMetrics value must be a boolean" };
            return nullopt;
        }
    }

    return { options };
}

} // namespace conversion
} // namespace style
} // namespace mbgl
