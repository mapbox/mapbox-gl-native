#pragma once

#include <mbgl/style/source_impl.hpp>
#include <mbgl/util/tileset.hpp>
#include <mbgl/util/variant.hpp>
#include <mbgl/util/optional.hpp>

namespace mbgl {

class AsyncRequest;

namespace style {

/*
   Shared implementation for VectorSource and RasterSource. Should eventually
   be refactored to use composition rather than inheritance.
*/
class TileSourceImpl : public Source::Impl {
public:
    static Tileset parseTileJSON(const std::string& json, const std::string& sourceURL, SourceType, uint16_t tileSize);

    TileSourceImpl(SourceType, std::string id, Source&,
                   variant<std::string, Tileset> urlOrTileset,
                   uint16_t tileSize);
    ~TileSourceImpl() override;

    void loadDescription(FileSource&) final;

    uint16_t getTileSize() const final {
        return tileSize;
    }

    const variant<std::string, Tileset>& getURLOrTileset() const {
        return urlOrTileset;
    }
    
    optional<std::string> getAttribution() const override;

protected:
    Range<uint8_t> getZoomRange() final;

    const variant<std::string, Tileset> urlOrTileset;
    const uint16_t tileSize;

    Tileset tileset;
    std::unique_ptr<AsyncRequest> req;
};

} // namespace style
} // namespace mbgl
