#pragma once

#include <mbgl/style/source.hpp>

#include <mbgl/util/rapidjson.hpp>

namespace mapbox {
namespace geojsonvt {
class GeoJSONVT;
} // namespace geojsonvt
} // namespace mapbox

namespace mbgl {

class AsyncRequest;

namespace style {

class GeoJSONSource : public Source {
public:
    static std::unique_ptr<GeoJSONSource> parse(const std::string& id,
                                                const JSValue&);

    GeoJSONSource(std::string id,
                  std::string url,
                  std::unique_ptr<mapbox::geojsonvt::GeoJSONVT>&&);
    ~GeoJSONSource() final;

    void load(FileSource&) final;

private:
    Range<uint8_t> getZoomRange() final;
    std::unique_ptr<Tile> createTile(const OverscaledTileID&, const UpdateParameters&) final;

    std::unique_ptr<mapbox::geojsonvt::GeoJSONVT> geojsonvt;
    std::unique_ptr<AsyncRequest> req;
};

} // namespace style
} // namespace mbgl
