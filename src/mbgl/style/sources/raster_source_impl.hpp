#pragma once

#include <mbgl/style/sources/raster_source.hpp>
#include <mbgl/style/source_impl.hpp>

namespace mbgl {
namespace style {

class RasterSource::Impl : public Source::Impl {
public:
    Impl(SourceType sourceType, std::string id, uint16_t tileSize);
    Impl(const Impl&, Tileset);

    uint16_t getTileSize() const;

    optional<std::string> getAttribution() const final;

    const optional<Tileset> tileset;

private:
    uint16_t tileSize;
};

} // namespace style
} // namespace mbgl
