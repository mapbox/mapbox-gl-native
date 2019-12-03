#include <mbgl/style/sources/geojson_source_impl.hpp>
#include <mbgl/tile/tile_id.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/feature.hpp>
#include <mbgl/util/string.hpp>
#include <mbgl/util/thread_pool.hpp>

#include <mapbox/geojsonvt.hpp>
#include <supercluster.hpp>

#include <cmath>

namespace mbgl {
namespace style {

class GeoJSONVTData : public GeoJSONData, public std::enable_shared_from_this<GeoJSONVTData> {
public:
    void getTile(const CanonicalTileID& id, const std::function<void(TileFeatures)>& fn) final {
        assert(fn);
        std::weak_ptr<GeoJSONVTData> weak = shared_from_this();
        scheduler->scheduleAndReplyValue(
            [id, weak, this]() -> TileFeatures {
                if (auto self = weak.lock()) {
                    return impl.getTile(id.z, id.x, id.y).features;
                }
                return {};
            },
            fn);
    }

    Features getChildren(const std::uint32_t) final { return {}; }

    Features getLeaves(const std::uint32_t, const std::uint32_t, const std::uint32_t) final { return {}; }

    std::uint8_t getClusterExpansionZoom(std::uint32_t) final {
        return 0;
    }

private:
    friend GeoJSONData;
    GeoJSONVTData(const GeoJSON& geoJSON, const mapbox::geojsonvt::Options& options)
        : impl(geoJSON, options), scheduler(Scheduler::GetSequenced()) {}
    mapbox::geojsonvt::GeoJSONVT impl;
    std::shared_ptr<Scheduler> scheduler;
};

class SuperclusterData : public GeoJSONData {
public:
    void getTile(const CanonicalTileID& id, const std::function<void(TileFeatures)>& fn) final {
        assert(fn);
        fn(impl.getTile(id.z, id.x, id.y));
    }

    Features getChildren(const std::uint32_t cluster_id) final { return impl.getChildren(cluster_id); }

    Features getLeaves(const std::uint32_t cluster_id, const std::uint32_t limit, const std::uint32_t offset) final {
        return impl.getLeaves(cluster_id, limit, offset);
    }

    std::uint8_t getClusterExpansionZoom(std::uint32_t cluster_id) final {
        return impl.getClusterExpansionZoom(cluster_id);
    }

private:
    friend GeoJSONData;
    SuperclusterData(const Features& features, const mapbox::supercluster::Options& options)
        : impl(features, options) {}
    mapbox::supercluster::Supercluster impl;
};

template <class T>
T evaluateFeature(const mapbox::feature::feature<double>& f,
                  const std::shared_ptr<expression::Expression>& expression,
                  optional<T> accumulated = nullopt) {
    const expression::EvaluationResult result = expression->evaluate(accumulated, f);
    if (result) {
        optional<T> typed = expression::fromExpressionValue<T>(*result);
        if (typed) {
            return std::move(*typed);
        }
    }
    return T();
}

// static
std::shared_ptr<GeoJSONData> GeoJSONData::create(const GeoJSON& geoJSON, Immutable<GeoJSONOptions> options) {
    constexpr double scale = util::EXTENT / util::tileSize;
    if (options->cluster && geoJSON.is<Features>() && !geoJSON.get<Features>().empty()) {
        mapbox::supercluster::Options clusterOptions;
        clusterOptions.maxZoom = options->clusterMaxZoom;
        clusterOptions.extent = util::EXTENT;
        clusterOptions.radius = ::round(scale * options->clusterRadius);
        auto feature = std::make_shared<Feature>();
        clusterOptions.map = [feature, options](const PropertyMap& properties) -> PropertyMap {
            PropertyMap ret{};
            if (properties.empty()) return ret;
            for (const auto& p : options->clusterProperties) {
                feature->properties = properties;
                ret[p.first] = evaluateFeature<Value>(*feature, p.second.first);
            }
            return ret;
        };
        clusterOptions.reduce = [feature, options](PropertyMap& toReturn, const PropertyMap& toFill) {
            for (const auto& p : options->clusterProperties) {
                if (toFill.count(p.first) == 0) {
                    continue;
                }
                feature->properties = toFill;
                optional<Value> accumulated(toReturn[p.first]);
                toReturn[p.first] = evaluateFeature<Value>(*feature, p.second.second, accumulated);
            }
        };
        return std::shared_ptr<GeoJSONData>(new SuperclusterData(geoJSON.get<Features>(), clusterOptions));
    }

    mapbox::geojsonvt::Options vtOptions;
    vtOptions.maxZoom = options->maxzoom;
    vtOptions.extent = util::EXTENT;
    vtOptions.buffer = ::round(scale * options->buffer);
    vtOptions.tolerance = scale * options->tolerance;
    vtOptions.lineMetrics = options->lineMetrics;
    return std::shared_ptr<GeoJSONData>(new GeoJSONVTData(geoJSON, vtOptions));
}

GeoJSONSource::Impl::Impl(std::string id_, Immutable<GeoJSONOptions> options_)
    : Source::Impl(SourceType::GeoJSON, std::move(id_)), options(std::move(options_)) {}

GeoJSONSource::Impl::Impl(const GeoJSONSource::Impl& other, std::shared_ptr<GeoJSONData> data_)
    : Source::Impl(other), options(other.options), data(std::move(data_)) {}

GeoJSONSource::Impl::~Impl() = default;

Range<uint8_t> GeoJSONSource::Impl::getZoomRange() const {
    return {options->minzoom, options->maxzoom};
}

std::weak_ptr<GeoJSONData> GeoJSONSource::Impl::getData() const {
    return data;
}

optional<std::string> GeoJSONSource::Impl::getAttribution() const {
    return {};
}

} // namespace style
} // namespace mbgl
