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
    void setGeoJSON(const GeoJSON&);

    std::string getURL();

    void load(FileSource&) final;

    uint16_t getTileSize() const final {
        return util::tileSize;
    }

private:
    Range<uint8_t> getZoomRange() final;
    std::unique_ptr<Tile> createTile(const OverscaledTileID&, const UpdateParameters&) final;

    variant<std::string, GeoJSONVTPointer, SuperclusterPointer> urlOrGeoJSON;
    std::unique_ptr<AsyncRequest> req;

    GeoJSONOptions options;
};

} // namespace style
} // namespace mbgl
