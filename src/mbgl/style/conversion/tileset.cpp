#include <mbgl/style/conversion/tileset.hpp>
#include <mbgl/util/geo.hpp>

namespace mbgl {
namespace style {
namespace conversion {

bool validateLatitude(const double lat) {
    return lat < 90 && lat > -90;
}

optional<Tileset> Converter<Tileset>::operator()(const Convertible& value, Error& error) const {
    Tileset result;

    auto tiles = objectMember(value, "tiles");
    if (!tiles) {
        error = { "source must have tiles" };
        return {};
    }

    if (!isArray(*tiles)) {
        error = { "source tiles must be an array" };
        return {};
    }

    for (std::size_t i = 0; i < arrayLength(*tiles); i++) {
        optional<std::string> urlTemplate = toString(arrayMember(*tiles, i));
        if (!urlTemplate) {
            error = { "source tiles member must be a string" };
            return {};
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

    auto minzoomValue = objectMember(value, "minzoom");
    if (minzoomValue) {
        optional<float> minzoom = toNumber(*minzoomValue);
        if (!minzoom || *minzoom < 0 || *minzoom > std::numeric_limits<uint8_t>::max()) {
            error = { "invalid minzoom" };
            return {};
        }
        result.zoomRange.min = *minzoom;
    }

    auto maxzoomValue = objectMember(value, "maxzoom");
    if (maxzoomValue) {
        optional<float> maxzoom = toNumber(*maxzoomValue);
        if (!maxzoom || *maxzoom < 0 || *maxzoom > std::numeric_limits<uint8_t>::max()) {
            error = { "invalid maxzoom" };
            return {};
        }
        result.zoomRange.max = *maxzoom;
    }

    auto attributionValue = objectMember(value, "attribution");
    if (attributionValue) {
        optional<std::string> attribution = toString(*attributionValue);
        if (!attribution) {
            error = { "source attribution must be a string" };
            return {};
        }
        result.attribution = std::move(*attribution);
    }

    auto boundsValue = objectMember(value, "bounds");
    if (boundsValue) {
        if (!isArray(*boundsValue) || arrayLength(*boundsValue) != 4) {
            error = { "bounds must be an array with left, bottom, top, and right values" };
            return {};
        }
        optional<double> left = toDouble(arrayMember(*boundsValue, 0));
        optional<double> bottom = toDouble(arrayMember(*boundsValue, 1));
        optional<double> right = toDouble(arrayMember(*boundsValue, 2));
        optional<double> top = toDouble(arrayMember(*boundsValue, 3));

        if (!left || !right || !bottom || !top) {
            error = { "bounds array must contain numeric longitude and latitude values" };
            return {};
        }
        if (!validateLatitude(*bottom) || !validateLatitude(*top) || top <= bottom){
            error = { "bounds latitude values must be between -90 and 90 with bottom less than top" };
            return {};
        }
        if(*left >= *right) {
            error = { "bounds left longitude should be less than right longitude" };
            return {};
        }
        result.bounds = LatLngBounds::hull({ *bottom, *left }, { *top, *right });
    }

    return result;
}

} // namespace conversion
} // namespace style
} // namespace mbgl
