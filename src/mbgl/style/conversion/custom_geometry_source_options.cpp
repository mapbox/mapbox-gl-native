#include <mbgl/style/conversion/custom_geometry_source_options.hpp>
#include <mbgl/style/conversion_impl.hpp>

namespace mbgl {
namespace style {
namespace conversion {

optional<CustomGeometrySource::Options> Converter<CustomGeometrySource::Options>::operator()(const Convertible& value, Error& error) const {
    CustomGeometrySource::Options options;

    const auto minzoomValue = objectMember(value, "minzoom");
    if (minzoomValue) {
        if (toNumber(*minzoomValue)) {
            options.zoomRange.min = static_cast<uint8_t>(*toNumber(*minzoomValue));
        } else {
            error.message = "GeoJSON source minzoom value must be a number";
            return nullopt;
        }
    }

    const auto maxzoomValue = objectMember(value, "maxzoom");
    if (maxzoomValue) {
        if (toNumber(*maxzoomValue)) {
            options.zoomRange.max = static_cast<uint8_t>(*toNumber(*maxzoomValue));
        } else {
            error.message = "GeoJSON source maxzoom value must be a number";
            return nullopt;
        }
    }

    const auto bufferValue = objectMember(value, "buffer");
    if (bufferValue) {
        if (toNumber(*bufferValue)) {
            options.tileOptions.buffer = static_cast<uint16_t>(*toNumber(*bufferValue));
        } else {
            error.message = "GeoJSON source buffer value must be a number";
            return nullopt;
        }
    }

    const auto toleranceValue = objectMember(value, "tolerance");
    if (toleranceValue) {
        if (toNumber(*toleranceValue)) {
            options.tileOptions.tolerance = static_cast<double>(*toNumber(*toleranceValue));
        } else {
            error.message = "GeoJSON source tolerance value must be a number";
            return nullopt;
        }
    }

    const auto wrapValue = objectMember(value, "wrap");
    if (wrapValue) {
        if (toBool(*wrapValue)) {
            options.tileOptions.wrap = static_cast<bool>(*toBool(*wrapValue));
        } else {
            error.message = "CustomGeometrySource TileOptions wrap value must be a boolean";
            return nullopt;
        }
    }

    const auto clipValue = objectMember(value, "clip");
    if (clipValue) {
        if (toBool(*clipValue)) {
            options.tileOptions.clip = static_cast<double>(*toBool(*clipValue));
        } else {
            error.message = "CustomGeometrySource TileOptiosn clip value must be a boolean";
            return nullopt;
        }
    }

    return options;
}

} // namespace conversion
} // namespace style
} // namespace mbgl
