#include <mbgl/style/conversion/tileset.hpp>
#include <mbgl/style/conversion_impl.hpp>
#include <mbgl/util/geo.hpp>
#include <mbgl/math/clamp.hpp>

namespace mbgl {
namespace style {
namespace conversion {

optional<Tileset> Converter<Tileset>::operator()(const Convertible& value, Error& error) const {
    Tileset result;

    auto tiles = objectMember(value, "tiles");
    if (!tiles) {
        error.message = "source must have tiles";
        return nullopt;
    }

    if (!isArray(*tiles)) {
        error.message = "source tiles must be an array";
        return nullopt;
    }

    for (std::size_t i = 0; i < arrayLength(*tiles); i++) {
        optional<std::string> urlTemplate = toString(arrayMember(*tiles, i));
        if (!urlTemplate) {
            error.message = "source tiles member must be a string";
            return nullopt;
        }
        result.tiles.push_back(std::move(*urlTemplate));
    }

    auto schemeValue = objectMember(value, "scheme");
    if (schemeValue) {
        optional<std::string> scheme = toString(*schemeValue);
        if (scheme && *scheme == "tms") {
            result.scheme = Tileset::Scheme::TMS;
        }
    }

    auto encodingValue = objectMember(value, "encoding");
    if (encodingValue) {
        optional<std::string> encoding = toString(*encodingValue);
        if (encoding && *encoding == "terrarium") {
            result.encoding = Tileset::DEMEncoding::Terrarium;
        } else if (encoding && *encoding != "mapbox") {
            error.message = "invalid raster-dem encoding type - valid types are 'mapbox' and 'terrarium' ";
        }
    }

    auto minzoomValue = objectMember(value, "minzoom");
    if (minzoomValue) {
        optional<float> minzoom = toNumber(*minzoomValue);
        if (!minzoom || *minzoom < 0 || *minzoom > std::numeric_limits<uint8_t>::max()) {
            error.message = "invalid minzoom";
            return nullopt;
        }
        result.zoomRange.min = *minzoom;
    }

    auto maxzoomValue = objectMember(value, "maxzoom");
    if (maxzoomValue) {
        optional<float> maxzoom = toNumber(*maxzoomValue);
        if (!maxzoom || *maxzoom < 0 || *maxzoom > std::numeric_limits<uint8_t>::max()) {
            error.message = "invalid maxzoom";
            return nullopt;
        }
        result.zoomRange.max = *maxzoom;
    }

    auto attributionValue = objectMember(value, "attribution");
    if (attributionValue) {
        optional<std::string> attribution = toString(*attributionValue);
        if (!attribution) {
            error.message = "source attribution must be a string";
            return nullopt;
        }
        result.attribution = std::move(*attribution);
    }

    auto boundsValue = objectMember(value, "bounds");
    if (boundsValue) {
        if (!isArray(*boundsValue) || arrayLength(*boundsValue) != 4) {
            error.message = "bounds must be an array with left, bottom, top, and right values";
            return nullopt;
        }
        optional<double> left = toDouble(arrayMember(*boundsValue, 0));
        optional<double> bottom = toDouble(arrayMember(*boundsValue, 1));
        optional<double> right = toDouble(arrayMember(*boundsValue, 2));
        optional<double> top = toDouble(arrayMember(*boundsValue, 3));

        if (!left || !right || !bottom || !top) {
            error.message = "bounds array must contain numeric longitude and latitude values";
            return nullopt;
        }

        bottom = util::clamp(*bottom, -90.0, 90.0);
        top = util::clamp(*top, -90.0, 90.0);
        if (*top < *bottom){
            error.message = "bounds bottom latitude must be less than or equal to top latitude";
            return nullopt;
        }

        if(*left > *right) {
            error.message = "bounds left longitude must be less than or equal to right longitude";
            return nullopt;
        }
        left = util::max(-180.0, *left);
        right = util::min(180.0, *right);
        result.bounds = LatLngBounds::hull({ *bottom, *left }, { *top, *right });
    }

    return result;
}

} // namespace conversion
} // namespace style
} // namespace mbgl
