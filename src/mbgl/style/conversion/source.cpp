#include <mbgl/style/conversion/source.hpp>
#include <mbgl/style/conversion/coordinate.hpp>
#include <mbgl/style/conversion/geojson.hpp>
#include <mbgl/style/conversion/geojson_options.hpp>
#include <mbgl/style/conversion/tileset.hpp>
#include <mbgl/style/sources/geojson_source.hpp>
#include <mbgl/style/sources/raster_source.hpp>
#include <mbgl/style/sources/raster_dem_source.hpp>
#include <mbgl/style/sources/vector_source.hpp>
#include <mbgl/style/sources/image_source.hpp>
#include <mbgl/util/geo.hpp>

namespace mbgl {
namespace style {
namespace conversion {

// A tile source can either specify a URL to TileJSON, or inline TileJSON.
static optional<variant<std::string, Tileset>> convertURLOrTileset(const Convertible& value, Error& error) {
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

static optional<std::unique_ptr<Source>> convertRasterSource(const std::string& id,
                                                             const Convertible& value,
                                                             Error& error) {
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

static optional<std::unique_ptr<Source>> convertRasterDEMSource(const std::string& id,
                                                             const Convertible& value,
                                                             Error& error) {
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

    return { std::make_unique<RasterDEMSource>(id, std::move(*urlOrTileset), tileSize) };
}

static optional<std::unique_ptr<Source>> convertVectorSource(const std::string& id,
                                                             const Convertible& value,
                                                             Error& error) {
    optional<variant<std::string, Tileset>> urlOrTileset = convertURLOrTileset(value, error);
    if (!urlOrTileset) {
        return {};
    }

    return { std::make_unique<VectorSource>(id, std::move(*urlOrTileset)) };
}

static optional<std::unique_ptr<Source>> convertGeoJSONSource(const std::string& id,
                                                              const Convertible& value,
                                                              Error& error) {
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

static optional<std::unique_ptr<Source>> convertImageSource(const std::string& id,
                                                            const Convertible& value,
                                                            Error& error) {
    auto urlValue = objectMember(value, "url");
    if (!urlValue) {
        error = { "Image source must have a url value" };
        return {};
    }
    
    auto urlString = toString(*urlValue);
    if (!urlString) {
        error = { "Image url must be a URL string" };
        return {};
    }
    
    auto coordinatesValue = objectMember(value, "coordinates");
    if (!coordinatesValue) {
        error = { "Image source must have a coordinates values" };
        return {};
    }
    
    if (!isArray(*coordinatesValue) || arrayLength(*coordinatesValue) != 4) {
        error = { "Image coordinates must be an array of four longitude latitude pairs" };
        return {};
    }
    
    std::array<LatLng, 4> coordinates;
    for (std::size_t i=0; i < 4; i++) {
        auto latLng = conversion::convert<LatLng>(arrayMember(*coordinatesValue,i), error);
        if (!latLng) {
            return {};
        }
        coordinates[i] = *latLng;
    }
    auto result = std::make_unique<ImageSource>(id, coordinates);
    result->setURL(*urlString);

    return { std::move(result) };
}

optional<std::unique_ptr<Source>> Converter<std::unique_ptr<Source>>::operator()(const Convertible& value, Error& error, const std::string& id) const {
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
    const std::string tname = *type;
    if (*type == "raster") {
        return convertRasterSource(id, value, error);
    } else if (*type == "raster-dem") {
        return convertRasterDEMSource(id, value, error);
    } else if (*type == "vector") {
        return convertVectorSource(id, value, error);
    } else if (*type == "geojson") {
        return convertGeoJSONSource(id, value, error);
    } else if (*type == "image") {
        return convertImageSource(id, value, error);
    } else {
        error = { "invalid source type" };
        return {};
    }
}

} // namespace conversion
} // namespace style
} // namespace mbgl
