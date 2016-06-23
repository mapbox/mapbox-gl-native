#pragma once

#include <mbgl/style/sources/geojson_source.hpp>
#include <mbgl/style/source_impl.hpp>
#include <mbgl/util/variant.hpp>

namespace mbgl {

class AsyncRequest;

namespace style {

class GeoJSONSource::Impl : public Source::Impl {
public:
    Impl(std::string id, Source&);
    ~Impl() final;

    void setURL(std::string);
    void setGeoJSON(GeoJSON&&);

    void load(FileSource&) final;

    uint16_t getTileSize() const final {
        return util::tileSize;
    }

    const variant<std::string, GeoJSON>& getURLOrGeoJSON() const {
        return urlOrGeoJSON;
    }

private:
    Range<uint8_t> getZoomRange() final;
    std::unique_ptr<Tile> createTile(const OverscaledTileID&, const UpdateParameters&) final;

    variant<std::string, GeoJSON> urlOrGeoJSON;
    std::unique_ptr<AsyncRequest> req;
};

} // namespace style
} // namespace mbgl
