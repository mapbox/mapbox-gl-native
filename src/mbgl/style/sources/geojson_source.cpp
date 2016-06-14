#include <mbgl/style/sources/geojson_source.hpp>
#include <mbgl/style/source_observer.hpp>
#include <mbgl/style/parser.hpp>
#include <mbgl/tile/geojson_tile.hpp>
#include <mbgl/storage/file_source.hpp>

#include <mapbox/geojsonvt.hpp>

#include <rapidjson/error/en.h>

#include <sstream>

namespace mbgl {
namespace style {

GeoJSONSource::GeoJSONSource(std::string id_,
                             std::string url_,
                             uint16_t tileSize_,
                             std::unique_ptr<Tileset>&& tileset_,
                             std::unique_ptr<mapbox::geojsonvt::GeoJSONVT>&& geojsonvt_)
    : Source(SourceType::GeoJSON, std::move(id_), std::move(url_), tileSize_, std::move(tileset_)),
      geojsonvt(std::move(geojsonvt_)) {
}

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
