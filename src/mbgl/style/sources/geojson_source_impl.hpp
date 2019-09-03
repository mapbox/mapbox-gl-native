#pragma once

#include <mbgl/style/source_impl.hpp>
#include <mbgl/style/sources/geojson_source.hpp>
#include <mbgl/util/range.hpp>

namespace mbgl {

class AsyncRequest;
class CanonicalTileID;

namespace style {

class GeoJSONData {
public:
    virtual ~GeoJSONData() = default;
    virtual mapbox::feature::feature_collection<int16_t> getTile(const CanonicalTileID&) = 0;

    // SuperclusterData
    virtual mapbox::feature::feature_collection<double> getChildren(const std::uint32_t) = 0;
    virtual mapbox::feature::feature_collection<double> getLeaves(const std::uint32_t,
                                                                   const std::uint32_t limit  = 10u,
                                                                   const std::uint32_t offset = 0u) = 0;
    virtual std::uint8_t getClusterExpansionZoom(std::uint32_t) = 0;
};

class GeoJSONSource::Impl : public Source::Impl {
public:
    Impl(std::string id, optional<GeoJSONOptions>);
    Impl(const GeoJSONSource::Impl&, const GeoJSON&);
    ~Impl() final;

    Range<uint8_t> getZoomRange() const;
    std::weak_ptr<GeoJSONData> getData() const;

    optional<std::string> getAttribution() const final;

private:
    GeoJSONOptions options;
    std::shared_ptr<GeoJSONData> data;
};

} // namespace style
} // namespace mbgl
