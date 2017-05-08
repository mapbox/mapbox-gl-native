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
    TileSourceImpl(SourceType, std::string id, Source&,
                   variant<std::string, Tileset> urlOrTileset,
                   uint16_t tileSize);
    ~TileSourceImpl() override;

    void loadDescription(FileSource&) final;

    uint16_t getTileSize() const {
        return tileSize;
    }

    const variant<std::string, Tileset>& getURLOrTileset() const {
        return urlOrTileset;
    }

    optional<std::string> getAttribution() const override;
    optional<Tileset> getTileset() const;

protected:
    const variant<std::string, Tileset> urlOrTileset;
    const uint16_t tileSize;

    Tileset tileset;
    std::unique_ptr<AsyncRequest> req;
};

} // namespace style
} // namespace mbgl
