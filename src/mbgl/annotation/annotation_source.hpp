#pragma once

#include <mbgl/style/source.hpp>

namespace mbgl {

class AnnotationSource : public style::Source {
public:
    AnnotationSource();

private:
    std::unique_ptr<Tile> createTile(const OverscaledTileID&, const style::UpdateParameters&) final;
};

} // namespace mbgl
