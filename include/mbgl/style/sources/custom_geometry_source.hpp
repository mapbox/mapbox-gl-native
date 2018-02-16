#pragma once

#include <mbgl/style/source.hpp>
#include <mbgl/util/geo.hpp>
#include <mbgl/util/geojson.hpp>
#include <mbgl/util/range.hpp>
#include <mbgl/util/constants.hpp>

namespace mbgl {

class OverscaledTileID;
class CanonicalTileID;
template <class T>
class Actor;

namespace style {

using TileFunction = std::function<void(const CanonicalTileID&)>;

class CustomTileLoader;

class CustomGeometrySource : public Source {
public:
    struct TileOptions {
        double tolerance = 0.375;
        uint16_t tileSize = util::tileSize;
        uint16_t buffer = 128;
        bool clip = false;
        bool wrap = false;
    };

    struct Options {
        TileFunction fetchTileFunction;
        TileFunction cancelTileFunction;
        Range<uint8_t> zoomRange = { 0, 18};
        TileOptions tileOptions;
    };
public:
    CustomGeometrySource(std::string id, CustomGeometrySource::Options options);
    ~CustomGeometrySource() final;
    void loadDescription(FileSource&) final;
    void setTileData(const CanonicalTileID&, const GeoJSON&);
    void invalidateTile(const CanonicalTileID&);
    void invalidateRegion(const LatLngBounds&);
    // Private implementation
    class Impl;
    const Impl& impl() const;
private:
    std::unique_ptr<Actor<CustomTileLoader>> loader;
};

template <>
inline bool Source::is<CustomGeometrySource>() const {
    return getType() == SourceType::CustomVector;
}

} // namespace style
} // namespace mbgl
