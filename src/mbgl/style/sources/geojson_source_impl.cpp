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

using namespace mapbox::geojsonvt;

namespace mbgl {
namespace style {
namespace conversion {

template <>
Result<GeoJSON> convertGeoJSON(const JSValue& value) {
    Options options;
    options.buffer = util::EXTENT / util::tileSize * 128;
    options.extent = util::EXTENT;

    try {
        const auto geojson = mapbox::geojson::convert(value);
        return GeoJSON { std::make_unique<GeoJSONVT>(geojson, options) };
    } catch (const std::exception& ex) {
        return Error { ex.what() };
    }
}
} // namespace conversion

GeoJSONSource::Impl::Impl(std::string id_, Source& base_)
    : Source::Impl(SourceType::GeoJSON, std::move(id_), base_) {
}

GeoJSONSource::Impl::~Impl() = default;

void GeoJSONSource::Impl::setURL(std::string url) {
    urlOrGeoJSON = std::move(url);
}

void GeoJSONSource::Impl::setGeoJSON(GeoJSON&& geoJSON) {
    urlOrGeoJSON = std::move(geoJSON);
}

void GeoJSONSource::Impl::load(FileSource& fileSource) {
    if (urlOrGeoJSON.is<GeoJSON>()) {
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

            conversion::Result<GeoJSON> geoJSON = conversion::convertGeoJSON<JSValue>(d);
            if (!geoJSON) {
                Log::Error(Event::ParseStyle, "Failed to parse GeoJSON data: %s", geoJSON.error().message);
                // Create an empty GeoJSON VT object to make sure we're not infinitely waiting for
                // tiles to load.
                mapbox::geojson::feature_collection features;
                urlOrGeoJSON = GeoJSON { std::make_unique<GeoJSONVT>(features) };
            } else {
                urlOrGeoJSON = std::move(*geoJSON);
            }

            loaded = true;
            observer->onSourceLoaded(base);
        }
    });
}

Range<uint8_t> GeoJSONSource::Impl::getZoomRange() {
    assert(loaded);
    return { 0, urlOrGeoJSON.get<GeoJSON>().impl->options.maxZoom };
}

std::unique_ptr<Tile> GeoJSONSource::Impl::createTile(const OverscaledTileID& tileID,
                                                      const UpdateParameters& parameters) {
    assert(loaded);
    return std::make_unique<GeoJSONTile>(tileID, base.getID(), parameters, *urlOrGeoJSON.get<GeoJSON>().impl);
}

} // namespace style
} // namespace mbgl
