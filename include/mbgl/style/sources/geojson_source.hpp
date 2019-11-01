#pragma once

#include <mbgl/style/expression/expression.hpp>
#include <mbgl/style/source.hpp>
#include <mbgl/tile/tile_id.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/geojson.hpp>
#include <mbgl/util/optional.hpp>

#include <memory>
#include <unordered_map>
#include <utility>

namespace mbgl {

class AsyncRequest;

namespace style {

struct GeoJSONOptions {
    // GeoJSON-VT options
    uint8_t minzoom = 0;
    uint8_t maxzoom = 18;
    uint16_t tileSize = util::tileSize;
    uint16_t buffer = 128;
    double tolerance = 0.375;
    bool lineMetrics = false;

    // Supercluster options
    bool cluster = false;
    uint16_t clusterRadius = 50;
    uint8_t clusterMaxZoom = 17;
    using ClusterExpression = std::pair<std::shared_ptr<mbgl::style::expression::Expression>,
                                        std::shared_ptr<mbgl::style::expression::Expression>>;
    using ClusterProperties = std::unordered_map<std::string, ClusterExpression>;
    ClusterProperties clusterProperties;
};
class GeoJSONData {
public:
    static std::shared_ptr<GeoJSONData> create(const GeoJSON&, const GeoJSONOptions&);

    virtual ~GeoJSONData() = default;
    virtual mapbox::feature::feature_collection<int16_t> getTile(const CanonicalTileID&) = 0;

    // SuperclusterData
    virtual mapbox::feature::feature_collection<double> getChildren(const std::uint32_t) = 0;
    virtual mapbox::feature::feature_collection<double> getLeaves(const std::uint32_t,
                                                                  const std::uint32_t limit = 10u,
                                                                  const std::uint32_t offset = 0u) = 0;
    virtual std::uint8_t getClusterExpansionZoom(std::uint32_t) = 0;
};

class GeoJSONSource final : public Source {
public:
    GeoJSONSource(const std::string& id, optional<GeoJSONOptions> = nullopt);
    ~GeoJSONSource() final;

    void setURL(const std::string& url);
    void setGeoJSON(const GeoJSON&);
    void setGeoJSONData(std::shared_ptr<GeoJSONData>);

    optional<std::string> getURL() const;
    const GeoJSONOptions& getOptions() const;

    class Impl;
    const Impl& impl() const;

    void loadDescription(FileSource&) final;

    bool supportsLayerType(const mbgl::style::LayerTypeInfo*) const override;

    mapbox::base::WeakPtr<Source> makeWeakPtr() override {
        return weakFactory.makeWeakPtr();
    }

private:
    optional<std::string> url;
    std::unique_ptr<AsyncRequest> req;
    mapbox::base::WeakPtrFactory<Source> weakFactory {this};
};

template <>
inline bool Source::is<GeoJSONSource>() const {
    return getType() == SourceType::GeoJSON;
}

} // namespace style
} // namespace mbgl
