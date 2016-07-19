#include <mbgl/style/sources/geojson_source_impl.hpp>
#include <mbgl/style/source_observer.hpp>
#include <mbgl/style/conversion/geojson.hpp>
#include <mbgl/tile/geojson_tile.hpp>
#include <mbgl/storage/file_source.hpp>
#include <mbgl/platform/log.hpp>
#include <mbgl/util/rapidjson.hpp>

#include <mapbox/geojson.hpp>
#include <mapbox/geojson/rapidjson.hpp>
#include <mapbox/geojsonvt.hpp>
#include <mapbox/geojsonvt/convert.hpp>

#include <rapidjson/error/en.h>

#include <sstream>

namespace mbgl {
namespace style {
namespace conversion {

template <>
Result<mapbox::geojson::geojson> convertGeoJSON(const JSValue& value) {
    try {
        return mapbox::geojson::convert(value);
    } catch (const std::exception& ex) {
        return Error { ex.what() };
    }
}
} // namespace conversion

GeoJSONSource::Impl::Impl(std::string id_, Source& base_, const GeoJSONOptions& options_)
    : Source::Impl(SourceType::GeoJSON, std::move(id_), base_), options(options_) {
}

GeoJSONSource::Impl::~Impl() = default;

void GeoJSONSource::Impl::setURL(std::string url) {
    urlOrGeoJSON = std::move(url);
}

void GeoJSONSource::Impl::setGeoJSON(mapbox::geojson::geojson&& geoJSON) {
    double scale = util::EXTENT / util::tileSize;

    mapbox::geojsonvt::Options vtOptions;
    vtOptions.maxZoom = options.maxzoom;
    vtOptions.extent = util::EXTENT;
    vtOptions.buffer = std::round(scale * options.buffer);
    vtOptions.tolerance = scale * options.tolerance;

    urlOrGeoJSON = std::make_unique<mapbox::geojsonvt::GeoJSONVT>(geoJSON, vtOptions);
}

void GeoJSONSource::Impl::load(FileSource& fileSource) {
    if (!urlOrGeoJSON.is<std::string>()) {
        loaded = true;
        return;
    }

    if (req) {
        return;
    }

    const std::string& url = urlOrGeoJSON.get<std::string>();
    req = fileSource.request(Resource::source(url), [this](Response res) {
        if (res.error) {
            observer->onSourceError(base, std::make_exception_ptr(std::runtime_error(res.error->message)));
        } else if (res.notModified) {
            return;
        } else if (res.noContent) {
            observer->onSourceError(base, std::make_exception_ptr(std::runtime_error("unexpectedly empty GeoJSON")));
        } else {
            rapidjson::GenericDocument<rapidjson::UTF8<>, rapidjson::CrtAllocator> d;
            d.Parse<0>(res.data->c_str());

            if (d.HasParseError()) {
                std::stringstream message;
                message << d.GetErrorOffset() << " - " << rapidjson::GetParseError_En(d.GetParseError());
                observer->onSourceError(base, std::make_exception_ptr(std::runtime_error(message.str())));
                return;
            }

            invalidateTiles();

            conversion::Result<mapbox::geojson::geojson> geoJSON = conversion::convertGeoJSON<JSValue>(d);
            if (!geoJSON) {
                Log::Error(Event::ParseStyle, "Failed to parse GeoJSON data: %s", geoJSON.error().message.c_str());
                // Create an empty GeoJSON VT object to make sure we're not infinitely waiting for
                // tiles to load.
                mapbox::geojson::feature_collection features;
                mapbox::geojson::geojson empty_geojson{ features };
                setGeoJSON(empty_geojson);
            } else {
                setGeoJSON(*geoJSON);
            }

            loaded = true;
            observer->onSourceLoaded(base);
        }
    });
}

Range<uint8_t> GeoJSONSource::Impl::getZoomRange() {
    assert(loaded);
    return { 0, urlOrGeoJSON.get<GeoJSONVTPointer>()->options.maxZoom };
}

std::unique_ptr<Tile> GeoJSONSource::Impl::createTile(const OverscaledTileID& tileID,
                                                      const UpdateParameters& parameters) {
    assert(loaded);
    return std::make_unique<GeoJSONTile>(tileID, base.getID(), parameters, *urlOrGeoJSON.get<GeoJSONVTPointer>());
}

} // namespace style
} // namespace mbgl
