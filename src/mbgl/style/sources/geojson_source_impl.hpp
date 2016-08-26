#pragma once

#include <mbgl/style/source_impl.hpp>
#include <mbgl/style/sources/geojson_source.hpp>
#include <mbgl/util/variant.hpp>

namespace mbgl {

class AsyncRequest;

namespace style {

class GeoJSONSource::Impl : public Source::Impl {
public:
    Impl(std::string id, Source&, const GeoJSONOptions);
    ~Impl() final;

    void setURL(std::string);
    optional<std::string> getURL();

    void setGeoJSON(const GeoJSON&);

    void load(FileSource&) final;

    uint16_t getTileSize() const final {
        return util::tileSize;
    }

private:
    Range<uint8_t> getZoomRange() final;
    std::unique_ptr<Tile> createTile(const OverscaledTileID&, const UpdateParameters&) final;

    GeoJSONOptions options;
    optional<std::string> url;
    std::unique_ptr<AsyncRequest> req;
    variant<GeoJSONVTPointer, SuperclusterPointer> geoJSONOrSupercluster;
};

} // namespace style
} // namespace mbgl
