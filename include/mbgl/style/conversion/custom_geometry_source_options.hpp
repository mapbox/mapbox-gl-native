#pragma once

#include <mbgl/style/conversion.hpp>
#include <mbgl/style/sources/custom_geometry_source.hpp>

namespace mbgl {
namespace style {
namespace conversion {

template <>
struct Converter<CustomGeometrySource::Options> {

    template <class V>
    optional<CustomGeometrySource::Options> operator()(const V& value, Error& error) const {
        CustomGeometrySource::Options options;

        const auto minzoomValue = objectMember(value, "minzoom");
        if (minzoomValue) {
            if (toNumber(*minzoomValue)) {
                options.zoomRange.min = static_cast<uint8_t>(*toNumber(*minzoomValue));
            } else {
                error = { "GeoJSON source minzoom value must be a number" };
                return {};
            }
        }

        const auto maxzoomValue = objectMember(value, "maxzoom");
        if (maxzoomValue) {
            if (toNumber(*maxzoomValue)) {
                options.zoomRange.max = static_cast<uint8_t>(*toNumber(*maxzoomValue));
            } else {
                error = { "GeoJSON source maxzoom value must be a number" };
                return {};
            }
        }

        const auto bufferValue = objectMember(value, "buffer");
        if (bufferValue) {
            if (toNumber(*bufferValue)) {
                options.tileOptions.buffer = static_cast<uint16_t>(*toNumber(*bufferValue));
            } else {
                error = { "GeoJSON source buffer value must be a number" };
                return {};
            }
        }

        const auto toleranceValue = objectMember(value, "tolerance");
        if (toleranceValue) {
            if (toNumber(*toleranceValue)) {
                options.tileOptions.tolerance = static_cast<double>(*toNumber(*toleranceValue));
            } else {
                error = { "GeoJSON source tolerance value must be a number" };
                return {};
            }
        }

        const auto wrapValue = objectMember(value, "wrap");
        if (wrapValue) {
            if (toBool(*wrapValue)) {
                options.tileOptions.wrap = static_cast<bool>(*toBool(*wrapValue));
            } else {
                error = { "CustomGeometrySource TileOptions wrap value must be a boolean" };
                return {};
            }
        }

        const auto clipValue = objectMember(value, "clip");
        if (clipValue) {
            if (toBool(*clipValue)) {
                options.tileOptions.clip = static_cast<double>(*toBool(*clipValue));
            } else {
                error = { "CustomGeometrySource TileOptiosn clip value must be a boolean" };
                return {};
            }
        }

        return { options };
    }

};

} // namespace conversion
} // namespace style
} // namespace mbgl
