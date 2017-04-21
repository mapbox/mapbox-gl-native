#pragma once

#include <mbgl/style/source_impl.hpp>
#include <mbgl/style/sources/geojson_source.hpp>
#include <mbgl/util/variant.hpp>

namespace mbgl {

class AsyncRequest;
class CanonicalTileID;

namespace style {

class GeoJSONData {
public:
    virtual mapbox::geometry::feature_collection<int16_t> getTile(const CanonicalTileID&) = 0;
};

class GeoJSONSource::Impl : public Source::Impl {
public:
    Impl(std::string id, Source&, const GeoJSONOptions);
    ~Impl() final;

    void setURL(std::string);
    optional<std::string> getURL() const;
    Range<uint8_t> getZoomRange() const;

    void setGeoJSON(const GeoJSON&);
    GeoJSONData* getData() const;

    void loadDescription(FileSource&) final;
    std::unique_ptr<RenderSource> createRenderSource() const final;

private:
    void _setGeoJSON(const GeoJSON&);

    GeoJSONOptions options;
    optional<std::string> url;
    std::unique_ptr<AsyncRequest> req;
    std::unique_ptr<GeoJSONData> data;
};

} // namespace style
} // namespace mbgl
