#pragma once

#include <mbgl/style/source_impl.hpp>
#include <mbgl/style/sources/geojson_source.hpp>
#include <mbgl/util/range.hpp>

namespace mbgl {

class AsyncRequest;
class CanonicalTileID;

namespace style {

class GeoJSONSource::Impl : public Source::Impl {
public:
    Impl(std::string id, Immutable<GeoJSONOptions>);
    Impl(const GeoJSONSource::Impl&, std::shared_ptr<GeoJSONData>);
    ~Impl() final;

    Range<uint8_t> getZoomRange() const;
    std::weak_ptr<GeoJSONData> getData() const;
    const Immutable<GeoJSONOptions>& getOptions() const { return options; }

    optional<std::string> getAttribution() const final;

private:
    Immutable<GeoJSONOptions> options;
    std::shared_ptr<GeoJSONData> data;
};

} // namespace style
} // namespace mbgl
