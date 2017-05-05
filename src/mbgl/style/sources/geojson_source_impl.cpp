#include <mbgl/style/sources/geojson_source_impl.hpp>
#include <mbgl/style/conversion/json.hpp>
#include <mbgl/style/conversion/geojson.hpp>
#include <mbgl/style/source_observer.hpp>
#include <mbgl/tile/tile_id.hpp>
#include <mbgl/storage/file_source.hpp>
#include <mbgl/renderer/sources/render_geojson_source.hpp>
#include <mbgl/util/constants.cpp>
#include <mbgl/util/logging.hpp>

#include <mapbox/geojsonvt.hpp>
#include <supercluster.hpp>

namespace mbgl {
namespace style {

class GeoJSONVTData : public GeoJSONData {
public:
    GeoJSONVTData(const GeoJSON& geoJSON,
                  const mapbox::geojsonvt::Options& options)
        : impl(geoJSON, options) {}

    mapbox::geometry::feature_collection<int16_t> getTile(const CanonicalTileID& tileID) final {
        return impl.getTile(tileID.z, tileID.x, tileID.y).features;
    }

private:
    mapbox::geojsonvt::GeoJSONVT impl;
};

class SuperclusterData : public GeoJSONData {
public:
    SuperclusterData(const mapbox::geometry::feature_collection<double>& features,
                     const mapbox::supercluster::Options& options)
        : impl(features, options) {}

    mapbox::geometry::feature_collection<int16_t> getTile(const CanonicalTileID& tileID) final {
        return impl.getTile(tileID.z, tileID.x, tileID.y);
    }

private:
    mapbox::supercluster::Supercluster impl;
};

GeoJSONSource::Impl::Impl(std::string id_, Source& base_, const GeoJSONOptions options_)
    : Source::Impl(SourceType::GeoJSON, std::move(id_), base_), options(options_) {
}

GeoJSONSource::Impl::~Impl() = default;

void GeoJSONSource::Impl::setURL(std::string url_) {
    url = std::move(url_);

    // Signal that the source description needs a reload
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

void GeoJSONSource::Impl::_setGeoJSON(const GeoJSON& geoJSON) {
    double scale = util::EXTENT / util::tileSize;

    if (options.cluster
        && geoJSON.is<mapbox::geometry::feature_collection<double>>()
        && !geoJSON.get<mapbox::geometry::feature_collection<double>>().empty()) {
        mapbox::supercluster::Options clusterOptions;
        clusterOptions.maxZoom = options.clusterMaxZoom;
        clusterOptions.extent = util::EXTENT;
        clusterOptions.radius = std::round(scale * options.clusterRadius);
        data = std::make_unique<SuperclusterData>(
            geoJSON.get<mapbox::geometry::feature_collection<double>>(), clusterOptions);
    } else {
        mapbox::geojsonvt::Options vtOptions;
        vtOptions.maxZoom = options.maxzoom;
        vtOptions.extent = util::EXTENT;
        vtOptions.buffer = std::round(scale * options.buffer);
        vtOptions.tolerance = scale * options.tolerance;
        data = std::make_unique<GeoJSONVTData>(geoJSON, vtOptions);
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
            conversion::Error error;
            optional<GeoJSON> geoJSON = conversion::convertJSON<GeoJSON>(*res.data, error);
            if (!geoJSON) {
                Log::Error(Event::ParseStyle, "Failed to parse GeoJSON data: %s",
                           error.message.c_str());
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

std::unique_ptr<RenderSource> GeoJSONSource::Impl::createRenderSource() const {
    return std::make_unique<RenderGeoJSONSource>(*this);
}

Range<uint8_t> GeoJSONSource::Impl::getZoomRange() const {
    return { 0, options.maxzoom };
}

GeoJSONData* GeoJSONSource::Impl::getData() const {
    return data.get();
}

} // namespace style
} // namespace mbgl
