#pragma once

#include <mbgl/style/conversion.hpp>
#include <mbgl/style/conversion/geojson.hpp>
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
    Result<std::unique_ptr<Source>> operator()(const V& value, const std::string& id) const {
        if (!isObject(value)) {
            return Error{ "source must be an object" };
        }

        auto typeValue = objectMember(value, "type");
        if (!typeValue) {
            return Error{ "source must have a type" };
        }

        optional<std::string> type = toString(*typeValue);
        if (!type) {
            return Error{ "source type must be a string" };
        }

        if (*type == "raster") {
            return convertRasterSource(id, value);
        } else if (*type == "vector") {
            return convertVectorSource(id, value);
        } else if (*type == "geojson") {
            return convertGeoJSONSource(id, value);
        } else {
            return Error{ "invalid source type" };
        }
    }

private:
    // A tile source can either specify a URL to TileJSON, or inline TileJSON.
    template <class V>
    Result<variant<std::string, Tileset>> convertURLOrTileset(const V& value) const {
        auto urlVal = objectMember(value, "url");
        if (!urlVal) {
            Result<Tileset> tileset = convert<Tileset>(value);
            if (!tileset) {
                return tileset.error();
            }
            return *tileset;
        }

        optional<std::string> url = toString(*urlVal);
        if (!url) {
            return Error{ "source url must be a string" };
        }

        return *url;
    }

    template <class V>
    Result<std::unique_ptr<Source>> convertRasterSource(const std::string& id,
                                                        const V& value) const {
        Result<variant<std::string, Tileset>> urlOrTileset = convertURLOrTileset(value);
        if (!urlOrTileset) {
            return urlOrTileset.error();
        }

        uint16_t tileSize = util::tileSize;
        auto tileSizeValue = objectMember(value, "tileSize");
        if (tileSizeValue) {
            optional<float> size = toNumber(*tileSizeValue);
            if (!size || *size < 0 || *size > std::numeric_limits<uint16_t>::max()) {
                return Error{ "invalid tileSize" };
            }
            tileSize = *size;
        }

        return std::make_unique<RasterSource>(id, std::move(*urlOrTileset), tileSize);
    }

    template <class V>
    Result<std::unique_ptr<Source>> convertVectorSource(const std::string& id,
                                                        const V& value) const {
        Result<variant<std::string, Tileset>> urlOrTileset = convertURLOrTileset(value);
        if (!urlOrTileset) {
            return urlOrTileset.error();
        }

        return std::make_unique<VectorSource>(id, std::move(*urlOrTileset));
    }

    template <class V>
    Result<std::unique_ptr<Source>> convertGeoJSONSource(const std::string& id,
                                                         const V& value) const {
        auto dataValue = objectMember(value, "data");
        if (!dataValue) {
            return Error{ "GeoJSON source must have a data value" };
        }

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

        auto result = std::make_unique<GeoJSONSource>(id, options);

        if (isObject(*dataValue)) {
            Result<GeoJSON> geoJSON = convertGeoJSON(*dataValue);
            if (!geoJSON) {
                return geoJSON.error();
            }
            result->setGeoJSON(std::move(*geoJSON));
        } else if (toString(*dataValue)) {
            result->setURL(*toString(*dataValue));
        } else {
            return Error{ "GeoJSON data must be a URL or an object" };
        }

        return std::move(result);
    }
};

} // namespace conversion
} // namespace style
} // namespace mbgl
