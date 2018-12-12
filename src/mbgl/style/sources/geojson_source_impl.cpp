#include <mbgl/style/sources/geojson_source_impl.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/tile/tile_id.hpp>
#include <mbgl/util/string.hpp>

#include <mapbox/geojsonvt.hpp>
#include <supercluster.hpp>

#include <cmath>

namespace mbgl {
namespace style {

class GeoJSONVTData : public GeoJSONData {
public:
    GeoJSONVTData(const GeoJSON& geoJSON,
                  const mapbox::geojsonvt::Options& options)
        : impl(geoJSON, options) {}

    mapbox::feature::feature_collection<int16_t> getTile(const CanonicalTileID& tileID) final {
        return impl.getTile(tileID.z, tileID.x, tileID.y).features;
    }

    mapbox::feature::feature_collection<double> getChildren(const std::uint32_t) final {
        return {};
    }

    mapbox::feature::feature_collection<double> getLeaves(const std::uint32_t,
                                                           const std::uint32_t,
                                                           const std::uint32_t) final {
        return {};
    }

    std::uint8_t getClusterExpansionZoom(std::uint32_t) final {
        return 0;
    }

private:
    mapbox::geojsonvt::GeoJSONVT impl;
};

class SuperclusterData : public GeoJSONData {
public:
    SuperclusterData(const mapbox::feature::feature_collection<double>& features,
                     const mapbox::supercluster::Options& options)
        : impl(features, options) {}

    mapbox::feature::feature_collection<int16_t> getTile(const CanonicalTileID& tileID) final {
        return impl.getTile(tileID.z, tileID.x, tileID.y);
    }

    mapbox::feature::feature_collection<double> getChildren(const std::uint32_t cluster_id) final {
        return impl.getChildren(cluster_id);
    }

    mapbox::feature::feature_collection<double> getLeaves(const std::uint32_t cluster_id,
                                                           const std::uint32_t limit,
                                                           const std::uint32_t offset) final {
        return impl.getLeaves(cluster_id, limit, offset);
    }

    std::uint8_t getClusterExpansionZoom(std::uint32_t cluster_id) final {
        return impl.getClusterExpansionZoom(cluster_id);
    }

private:
    mapbox::supercluster::Supercluster impl;
};

GeoJSONSource::Impl::Impl(std::string id_, GeoJSONOptions options_)
    : Source::Impl(SourceType::GeoJSON, std::move(id_)),
      options(std::move(options_)) {
}

GeoJSONSource::Impl::Impl(const Impl& other, const GeoJSON& geoJSON)
    : Source::Impl(other),
      options(other.options) {
    double scale = util::EXTENT / util::tileSize;

    if (options.cluster
        && geoJSON.is<mapbox::feature::feature_collection<double>>()
        && !geoJSON.get<mapbox::feature::feature_collection<double>>().empty()) {
        mapbox::supercluster::Options clusterOptions;
        clusterOptions.maxZoom = options.clusterMaxZoom;
        clusterOptions.extent = util::EXTENT;
        clusterOptions.radius = ::round(scale * options.clusterRadius);
        data = std::make_unique<SuperclusterData>(
            geoJSON.get<mapbox::feature::feature_collection<double>>(), clusterOptions);
    } else {
        mapbox::geojsonvt::Options vtOptions;
        vtOptions.maxZoom = options.maxzoom;
        vtOptions.extent = util::EXTENT;
        vtOptions.buffer = ::round(scale * options.buffer);
        vtOptions.tolerance = scale * options.tolerance;
        vtOptions.lineMetrics = options.lineMetrics;
        data = std::make_unique<GeoJSONVTData>(geoJSON, vtOptions);
    }
}

GeoJSONSource::Impl::~Impl() = default;

Range<uint8_t> GeoJSONSource::Impl::getZoomRange() const {
    return { options.minzoom, options.maxzoom };
}

GeoJSONData* GeoJSONSource::Impl::getData() const {
    return data.get();
}

optional<std::string> GeoJSONSource::Impl::getAttribution() const {
    return {};
}

} // namespace style
} // namespace mbgl
