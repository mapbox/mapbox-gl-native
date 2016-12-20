#include <mbgl/util/logging.hpp>
#include <mbgl/storage/file_source.hpp>
#include <mbgl/style/conversion/geojson.hpp>
#include <mbgl/style/source_observer.hpp>
#include <mbgl/style/sources/geojson_source_impl.hpp>
#include <mbgl/tile/geojson_tile.hpp>
#include <mbgl/util/rapidjson.hpp>

#include <mbgl/util/string.hpp>
#include <mapbox/geojson.hpp>
#include <mapbox/geojson/rapidjson.hpp>
#include <mapbox/geojsonvt.hpp>
#include <mapbox/geojsonvt/convert.hpp>
#include <supercluster.hpp>

#include <rapidjson/error/en.h>

#include <cmath>
#include <sstream>

namespace mbgl {
namespace style {
namespace conversion {

template <>
Result<GeoJSON> convertGeoJSON(const JSValue& value) {
    try {
        return mapbox::geojson::convert(value);
    } catch (const std::exception& ex) {
        return Error{ ex.what() };
    }
}
} // namespace conversion

GeoJSONSource::Impl::Impl(std::string id_, Source& base_, const GeoJSONOptions options_)
    : Source::Impl(SourceType::GeoJSON, std::move(id_), base_), options(options_) {
}

GeoJSONSource::Impl::~Impl() = default;

void GeoJSONSource::Impl::setURL(std::string url_) {
    url = std::move(url_);

    //Signal that the source description needs a reload
    if (loaded || req) {
        loaded = false;
        req.reset();
        observer->onSourceDescriptionChanged(base);
    }
}

optional<std::string> GeoJSONSource::Impl::getURL() const {
    return url;
}


void GeoJSONSource::Impl::setGeoJSON(const GeoJSON& geoJSON) {
    req.reset();
    _setGeoJSON(geoJSON);
}

//Private implementation
void GeoJSONSource::Impl::_setGeoJSON(const GeoJSON& geoJSON) {
    double scale = util::EXTENT / util::tileSize;

    cache.clear();

    if (options.cluster
        && geoJSON.is<mapbox::geometry::feature_collection<double>>()
        && !geoJSON.get<mapbox::geometry::feature_collection<double>>().empty()) {
        mapbox::supercluster::Options clusterOptions;
        clusterOptions.maxZoom = options.clusterMaxZoom;
        clusterOptions.extent = util::EXTENT;
        clusterOptions.radius = ::round(scale * options.clusterRadius);

        const auto& features = geoJSON.get<mapbox::geometry::feature_collection<double>>();
        geoJSONOrSupercluster =
            std::make_unique<mapbox::supercluster::Supercluster>(features, clusterOptions);
    } else {
        mapbox::geojsonvt::Options vtOptions;
        vtOptions.maxZoom = options.maxzoom;
        vtOptions.extent = util::EXTENT;
        vtOptions.buffer = ::round(scale * options.buffer);
        vtOptions.tolerance = scale * options.tolerance;
        geoJSONOrSupercluster = std::make_unique<mapbox::geojsonvt::GeoJSONVT>(geoJSON, vtOptions);
    }

    for (auto const &item : tiles) {
        GeoJSONTile* geoJSONTile = static_cast<GeoJSONTile*>(item.second.get());
        setTileData(*geoJSONTile, geoJSONTile->id);
    }
}

void GeoJSONSource::Impl::setTileData(GeoJSONTile& tile, const OverscaledTileID& tileID) {
    if (geoJSONOrSupercluster.is<GeoJSONVTPointer>()) {
        tile.updateData(geoJSONOrSupercluster.get<GeoJSONVTPointer>()->getTile(tileID.canonical.z,
                                                                               tileID.canonical.x,
                                                                               tileID.canonical.y).features);
    } else {
        assert(geoJSONOrSupercluster.is<SuperclusterPointer>());
        tile.updateData(geoJSONOrSupercluster.get<SuperclusterPointer>()->getTile(tileID.canonical.z,
                                                                                  tileID.canonical.x,
                                                                                  tileID.canonical.y));
    }
}

void GeoJSONSource::Impl::loadDescription(FileSource& fileSource) {
    if (!url) {
        loaded = true;
        return;
    }

    if (req) {
        return;
    }

    req = fileSource.request(Resource::source(*url), [this](Response res) {
        if (res.error) {
            observer->onSourceError(
                base, std::make_exception_ptr(std::runtime_error(res.error->message)));
        } else if (res.notModified) {
            return;
        } else if (res.noContent) {
            observer->onSourceError(
                base, std::make_exception_ptr(std::runtime_error("unexpectedly empty GeoJSON")));
        } else {
            rapidjson::GenericDocument<rapidjson::UTF8<>, rapidjson::CrtAllocator> d;
            d.Parse<0>(res.data->c_str());

            if (d.HasParseError()) {
                std::stringstream message;
                message << d.GetErrorOffset() << " - "
                        << rapidjson::GetParseError_En(d.GetParseError());
                observer->onSourceError(base,
                                        std::make_exception_ptr(std::runtime_error(message.str())));
                return;
            }

            invalidateTiles();

            conversion::Result<GeoJSON> geoJSON = conversion::convertGeoJSON<JSValue>(d);
            if (!geoJSON) {
                Log::Error(Event::ParseStyle, "Failed to parse GeoJSON data: %s",
                           geoJSON.error().message.c_str());
                // Create an empty GeoJSON VT object to make sure we're not infinitely waiting for
                // tiles to load.
                _setGeoJSON(GeoJSON{ FeatureCollection{} });
            } else {
                _setGeoJSON(*geoJSON);
            }

            loaded = true;
            observer->onSourceLoaded(base);
        }
    });
}

Range<uint8_t> GeoJSONSource::Impl::getZoomRange() {
    assert(loaded);
    return { 0, options.maxzoom };
}

std::unique_ptr<Tile> GeoJSONSource::Impl::createTile(const OverscaledTileID& tileID,
                                                      const UpdateParameters& parameters) {
    assert(loaded);
    auto tilePointer = std::make_unique<GeoJSONTile>(tileID, base.getID(), parameters);
    setTileData(*tilePointer.get(), tileID);
    return std::move(tilePointer);
}

} // namespace style
} // namespace mbgl
