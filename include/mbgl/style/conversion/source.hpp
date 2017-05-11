#pragma once

#include <mbgl/style/conversion.hpp>
#include <mbgl/style/conversion/geojson.hpp>
#include <mbgl/style/conversion/geojson_options.hpp>
#include <mbgl/style/conversion/tileset.hpp>
#include <mbgl/style/source.hpp>
#include <mbgl/style/sources/geojson_source.hpp>
#include <mbgl/style/sources/raster_source.hpp>
#include <mbgl/style/sources/vector_source.hpp>

namespace mbgl {
namespace style {
namespace conversion {

template <>
struct Converter<std::unique_ptr<Source>> {
public:
    template <class V>
    optional<std::unique_ptr<Source>> operator()(const V& value, Error& error, const std::string& id) const {
        if (!isObject(value)) {
            error = { "source must be an object" };
            return {};
        }

        auto typeValue = objectMember(value, "type");
        if (!typeValue) {
            error = { "source must have a type" };
            return {};
        }

        optional<std::string> type = toString(*typeValue);
        if (!type) {
            error = { "source type must be a string" };
            return {};
        }

        if (*type == "raster") {
            return convertRasterSource(id, value, error);
        } else if (*type == "vector") {
            return convertVectorSource(id, value, error);
        } else if (*type == "geojson") {
            return convertGeoJSONSource(id, value, error);
        } else {
            error = { "invalid source type" };
            return {};
        }
    }

private:
    // A tile source can either specify a URL to TileJSON, or inline TileJSON.
    template <class V>
    optional<variant<std::string, Tileset>> convertURLOrTileset(const V& value, Error& error) const {
        auto urlVal = objectMember(value, "url");
        if (!urlVal) {
            optional<Tileset> tileset = convert<Tileset>(value, error);
            if (!tileset) {
                return {};
            }
            return { *tileset };
        }

        optional<std::string> url = toString(*urlVal);
        if (!url) {
            error = { "source url must be a string" };
            return {};
        }

        return { *url };
    }

    template <class V>
    optional<std::unique_ptr<Source>> convertRasterSource(const std::string& id,
                                                          const V& value,
                                                          Error& error) const {
        optional<variant<std::string, Tileset>> urlOrTileset = convertURLOrTileset(value, error);
        if (!urlOrTileset) {
            return {};
        }

        uint16_t tileSize = util::tileSize;
        auto tileSizeValue = objectMember(value, "tileSize");
        if (tileSizeValue) {
            optional<float> size = toNumber(*tileSizeValue);
            if (!size || *size < 0 || *size > std::numeric_limits<uint16_t>::max()) {
                error = { "invalid tileSize" };
                return {};
            }
            tileSize = *size;
        }

        return { std::make_unique<RasterSource>(id, std::move(*urlOrTileset), tileSize) };
    }

    template <class V>
    optional<std::unique_ptr<Source>> convertVectorSource(const std::string& id,
                                                          const V& value,
                                                          Error& error) const {
        optional<variant<std::string, Tileset>> urlOrTileset = convertURLOrTileset(value, error);
        if (!urlOrTileset) {
            return {};
        }

        return { std::make_unique<VectorSource>(id, std::move(*urlOrTileset)) };
    }

    template <class V>
    optional<std::unique_ptr<Source>> convertGeoJSONSource(const std::string& id,
                                                           const V& value,
                                                           Error& error) const {
        auto dataValue = objectMember(value, "data");
        if (!dataValue) {
            error = { "GeoJSON source must have a data value" };
            return {};
        }

        optional<GeoJSONOptions> options = convert<GeoJSONOptions>(value, error);
        if (!options) {
            return {};
        }

        auto result = std::make_unique<GeoJSONSource>(id, *options);

        if (isObject(*dataValue)) {
            optional<GeoJSON> geoJSON = convert<GeoJSON>(*dataValue, error);
            if (!geoJSON) {
                return {};
            }
            result->setGeoJSON(std::move(*geoJSON));
        } else if (toString(*dataValue)) {
            result->setURL(*toString(*dataValue));
        } else {
            error = { "GeoJSON data must be a URL or an object" };
            return {};
        }

        return { std::move(result) };
    }
};

} // namespace conversion
} // namespace style
} // namespace mbgl
