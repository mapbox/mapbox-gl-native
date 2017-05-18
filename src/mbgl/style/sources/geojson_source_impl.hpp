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
    virtual mapbox::geometry::feature_collection<int16_t> getTile(const CanonicalTileID&) = 0;
};

class GeoJSONSource::Impl : public Source::Impl {
public:
    Impl(std::string id, GeoJSONOptions);
    Impl(const GeoJSONSource::Impl&, const GeoJSON&);
    ~Impl() final;

    Range<uint8_t> getZoomRange() const;
    GeoJSONData* getData() const;

    optional<std::string> getAttribution() const final;

private:
    GeoJSONOptions options;
    std::unique_ptr<GeoJSONData> data;
};

} // namespace style
} // namespace mbgl
