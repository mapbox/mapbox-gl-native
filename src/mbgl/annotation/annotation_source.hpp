#pragma once

#include <mbgl/style/source.hpp>
#include <mbgl/style/source_impl.hpp>

namespace mbgl {

class AnnotationSource : public style::Source {
public:
    AnnotationSource();

    class Impl;
};

class AnnotationSource::Impl : public style::Source::Impl {
public:
    Impl(Source&);

    void loadDescription(FileSource&) final;

private:
    uint16_t getTileSize() const final { return util::tileSize; }
    Range<uint8_t> getZoomRange() final;

    std::unique_ptr<Tile> createTile(const OverscaledTileID&, const style::UpdateParameters&) final;
};

} // namespace mbgl
