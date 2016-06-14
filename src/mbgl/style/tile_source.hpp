#pragma once

#include <mbgl/style/source.hpp>

namespace mbgl {

class Tileset;
class AsyncRequest;

namespace style {

/*
   Shared implementation for VectorSource and RasterSource. Should eventually
   be refactored to use composition rather than inheritance.
*/
class TileSource : public Source {
public:
    TileSource(SourceType,
               std::string id,
               std::string url,
               uint16_t tileSize,
               std::unique_ptr<Tileset>&&);
    ~TileSource() override;

    void load(FileSource&) final;

    const std::string& getURL() const { return url; }
    const Tileset* getTileset() const { return tileset.get(); }

protected:
    Range<uint8_t> getZoomRange() final;

    const std::string url;
    std::unique_ptr<const Tileset> tileset;
    std::unique_ptr<AsyncRequest> req;
};

} // namespace style
} // namespace mbgl
