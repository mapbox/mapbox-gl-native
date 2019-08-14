#include <mbgl/style/conversion/geojson_options.hpp>
#include <mbgl/style/conversion_impl.hpp>
#include <mbgl/style/expression/dsl.hpp>

#include <sstream>

namespace mbgl {
namespace style {
namespace conversion {

optional<GeoJSONOptions> Converter<GeoJSONOptions>::operator()(const Convertible& value,
                                                               Error& error) const {
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
            error.message = "GeoJSON source lineMetrics value must be a boolean";
            return nullopt;
        }
    }

    const auto clusterProperties = objectMember(value, "clusterProperties");
    if (clusterProperties) {
        if (!isObject(*clusterProperties)) {
            error.message = "GeoJSON source clusterProperties value must be an object";
            return nullopt;
        }
        GeoJSONOptions::ClusterProperties result;
        assert(error.message.empty());
        eachMember(
            *clusterProperties,
            [&](const std::string& k,
                const mbgl::style::conversion::Convertible& v) -> optional<conversion::Error> {
                // Each property shall be formed as ["key" : [operator, [mapExpression]]]
                // or ["key" : [[operator, ['accumulated'], ['get', key]], [mapExpression]]]
                if (!isArray(v) || arrayLength(v) != 2) {
                    error.message =
                        "GeoJSON source clusterProperties member must be an array with length of 2";
                    return nullopt;
                }
                auto map = expression::dsl::createExpression(arrayMember(v, 1));
                if (!map) {
                    error.message =
                        "Failed to convert GeoJSON source clusterProperties map expression";
                    return nullopt;
                }
                std::unique_ptr<expression::Expression> reduce;
                if (isArray(arrayMember(v, 0))) {
                    reduce = expression::dsl::createExpression(arrayMember(v, 0));
                } else {
                    auto reduceOp = toString(arrayMember(v, 0));
                    if (!reduceOp) {
                        error.message =
                            "GeoJSON source clusterProperties member must contain a valid operator";
                        return nullopt;
                    }
                    std::stringstream ss;
                    // Reformulate reduce expression to [operator, ['accumulated'], ['get', key]]
                    // The reason to create expression via parsing string instead of invoking function
                    // createCompoundExpression is due to expression type disunity can’t be resolved
                    // with current logic of createCompoundExpression
                    ss << std::string(R"([")") << *reduceOp
                       << std::string(R"(", ["accumulated"], ["get", ")") << k
                       << std::string(R"("]])");
                    reduce = expression::dsl::createExpression(ss.str().c_str());
                }
                if (!reduce) {
                    error.message =
                        "Failed to convert GeoJSON source clusterProperties reduce expression";
                    return nullopt;
                }
                result.emplace(k, std::make_pair(std::move(map), std::move(reduce)));
                return nullopt;
            });
        if (!error.message.empty()) {
            return nullopt;
        }
        options.clusterProperties = std::move(result);
    }

    return { std::move(options) };
}

} // namespace conversion
} // namespace style
} // namespace mbgl
