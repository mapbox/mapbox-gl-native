#include <mbgl/style/sources/geojson_source.hpp>
#include <mbgl/style/source_observer.hpp>
#include <mbgl/style/parser.hpp>
#include <mbgl/tile/geojson_tile.hpp>
#include <mbgl/storage/file_source.hpp>
#include <mbgl/platform/log.hpp>

#include <mapbox/geojsonvt.hpp>
#include <mapbox/geojsonvt/convert.hpp>

#include <rapidjson/error/en.h>

#include <sstream>

namespace mbgl {
namespace style {

std::unique_ptr<mapbox::geojsonvt::GeoJSONVT> parseGeoJSON(const JSValue& value) {
    using namespace mapbox::geojsonvt;

    Options options;
    options.buffer = util::EXTENT / util::tileSize * 128;
    options.extent = util::EXTENT;

    try {
        return std::make_unique<GeoJSONVT>(Convert::convert(value, 0), options);
    } catch (const std::exception& ex) {
        Log::Error(Event::ParseStyle, "Failed to parse GeoJSON data: %s", ex.what());
        // Create an empty GeoJSON VT object to make sure we're not infinitely waiting for
        // tiles to load.
        return std::make_unique<GeoJSONVT>(std::vector<ProjectedFeature>{}, options);
    }
}

std::unique_ptr<GeoJSONSource> GeoJSONSource::parse(const std::string& id,
                                                    const JSValue& value) {
    auto tileset = std::make_unique<Tileset>();
    std::unique_ptr<mapbox::geojsonvt::GeoJSONVT> geojsonvt;
    std::string url;

    // We should probably split this up to have URLs in the url property, and actual data
    // in the data property. Until then, we're going to detect the content based on the
    // object type.
    if (!value.HasMember("data")) {
        Log::Error(Event::ParseStyle, "GeoJSON source must have a data value");
        return nullptr;
    }

    const JSValue& dataVal = value["data"];
    if (dataVal.IsString()) {
        // We need to load an external GeoJSON file
        url = { dataVal.GetString(), dataVal.GetStringLength() };
    } else if (dataVal.IsObject()) {
        // We need to parse dataVal as a GeoJSON object
        geojsonvt = parseGeoJSON(dataVal);
        tileset->maxZoom = geojsonvt->options.maxZoom;
    } else {
        Log::Error(Event::ParseStyle, "GeoJSON data must be a URL or an object");
        return nullptr;
    }

    return std::make_unique<GeoJSONSource>(id, url, std::move(tileset), std::move(geojsonvt));
}

GeoJSONSource::GeoJSONSource(std::string id_,
                             std::string url_,
                             std::unique_ptr<Tileset>&& tileset_,
                             std::unique_ptr<mapbox::geojsonvt::GeoJSONVT>&& geojsonvt_)
    : Source(SourceType::GeoJSON, std::move(id_), std::move(url_), util::tileSize, std::move(tileset_)),
      geojsonvt(std::move(geojsonvt_)) {
}

GeoJSONSource::~GeoJSONSource() = default;

void GeoJSONSource::load(FileSource& fileSource) {
    if (url.empty()) {
        // If the URL is empty, the GeoJSON was specified inline in the stylesheet.
        loaded = true;
        return;
    }

    if (req) {
        return;
    }

    req = fileSource.request(Resource::source(url), [this](Response res) {
        if (res.error) {
            observer->onSourceError(*this, std::make_exception_ptr(std::runtime_error(res.error->message)));
        } else if (res.notModified) {
            return;
        } else if (res.noContent) {
            observer->onSourceError(*this, std::make_exception_ptr(std::runtime_error("unexpectedly empty GeoJSON")));
        } else {
            std::unique_ptr<Tileset> newTileset = std::make_unique<Tileset>();

            rapidjson::GenericDocument<rapidjson::UTF8<>, rapidjson::CrtAllocator> d;
            d.Parse<0>(res.data->c_str());

            if (d.HasParseError()) {
                std::stringstream message;
                message << d.GetErrorOffset() << " - " << rapidjson::GetParseError_En(d.GetParseError());
                observer->onSourceError(*this, std::make_exception_ptr(std::runtime_error(message.str())));
                return;
            }

            geojsonvt = style::parseGeoJSON(d);
            newTileset->maxZoom = geojsonvt->options.maxZoom;

            invalidateTiles();

            tileset = std::move(newTileset);
            loaded = true;
            observer->onSourceLoaded(*this);
        }
    });
}

std::unique_ptr<Tile> GeoJSONSource::createTile(const OverscaledTileID& tileID,
                                                const UpdateParameters& parameters) {
    return std::make_unique<GeoJSONTile>(tileID, id, parameters, geojsonvt.get());
}

} // namespace style
} // namespace mbgl
