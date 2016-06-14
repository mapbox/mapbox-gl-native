#pragma once

#include <mbgl/style/source.hpp>
#include <mbgl/util/tileset.hpp>
#include <mbgl/util/variant.hpp>
#include <mbgl/util/optional.hpp>
#include <mbgl/util/rapidjson.hpp>

namespace mbgl {

class AsyncRequest;

namespace style {

/*
   Shared implementation for VectorSource and RasterSource. Should eventually
   be refactored to use composition rather than inheritance.
*/
class TileSource : public Source {
public:
    // A tile source can either specify a URL to TileJSON, or inline TileJSON.
    static optional<variant<std::string, Tileset>> parseURLOrTileset(const JSValue&);
    static Tileset parseTileJSON(const std::string& json, const std::string& sourceURL, SourceType, uint16_t tileSize);

    TileSource(SourceType,
               std::string id,
               variant<std::string, Tileset> urlOrTileset,
               uint16_t tileSize);
    ~TileSource() override;

    void load(FileSource&) final;

    uint16_t getTileSize() const final {
        return tileSize;
    }

    const variant<std::string, Tileset>& getURLOrTileset() const {
        return urlOrTileset;
    }

protected:
    Range<uint8_t> getZoomRange() final;

    const variant<std::string, Tileset> urlOrTileset;
    const uint16_t tileSize;

    Tileset tileset;
    std::unique_ptr<AsyncRequest> req;
};

} // namespace style
} // namespace mbgl
