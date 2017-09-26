#pragma once

#include <mbgl/util/tileset.hpp>
#include <mbgl/style/conversion.hpp>

namespace mbgl {
namespace style {
namespace conversion {

template <>
struct Converter<Tileset> {
public:
    optional<Tileset> operator()(const Value& value, Error& error) const {
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

        return result;
    }
};

} // namespace conversion
} // namespace style
} // namespace mbgl
