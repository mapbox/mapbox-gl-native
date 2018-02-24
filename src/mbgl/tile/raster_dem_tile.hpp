#pragma once

#include <mbgl/tile/tile.hpp>
#include <mbgl/tile/tile_loader.hpp>
#include <mbgl/tile/raster_dem_tile_worker.hpp>
#include <mbgl/actor/actor.hpp>

namespace mbgl {

class Tileset;
class TileParameters;
class HillshadeBucket;

enum class DEMTileNeighbors : uint8_t {
  // 0b00000000
  Empty = 0 << 1,

  // 0b00000001
  Left = 1 << 0,
  // 0b00000010
  Right = 1 << 1,
  // 0b00000100
  TopLeft = 1 << 2,
  // 0b00001000
  TopCenter = 1 << 3,
  // 0b00010000
  TopRight = 1 << 4,
  // 0b00100000
  BottomLeft = 1 << 5,
  // 0b01000000
  BottomCenter = 1 << 6,
  // 0b10000000
  BottomRight = 1 << 7,

  // helper enums for tiles with no upper/lower neighbors
  // and completely backfilled tiles

  // 0b00011100
  NoUpper = 0b00011100,
  // 0b11100000
  NoLower = 0b11100000,
  // 0b11111111
  Complete = 0b11111111
};

inline DEMTileNeighbors operator|(DEMTileNeighbors a, DEMTileNeighbors b) {
    return static_cast<DEMTileNeighbors>(int(a) | int(b));
};

inline DEMTileNeighbors operator&(DEMTileNeighbors a, DEMTileNeighbors b) {
    return static_cast<DEMTileNeighbors>(int(a) & int(b));
}

inline bool operator!=(DEMTileNeighbors a, DEMTileNeighbors b) {
    return static_cast<unsigned char>(a) != static_cast<unsigned char>(b);
}

namespace style {
class Layer;
} // namespace style

class RasterDEMTile : public Tile {
public:
    RasterDEMTile(const OverscaledTileID&,
                   const TileParameters&,
                   const Tileset&);
    ~RasterDEMTile() override;

    void setNecessity(TileNecessity) final;

    void setError(std::exception_ptr);
    void setMetadata(optional<Timestamp> modified, optional<Timestamp> expires);
    void setData(std::shared_ptr<const std::string> data);

    void upload(gl::Context&) override;
    Bucket* getBucket(const style::Layer::Impl&) const override;

    HillshadeBucket* getBucket() const;
    void backfillBorder(const RasterDEMTile& borderTile, const DEMTileNeighbors mask);
    
    // neighboringTiles is a bitmask for which neighboring tiles have been backfilled
    // there are max 8 possible neighboring tiles, so each bit represents one neighbor
    DEMTileNeighbors neighboringTiles = DEMTileNeighbors::Empty;
    
    void setMask(TileMask&&) override;

    void onParsed(std::unique_ptr<HillshadeBucket> result, uint64_t correlationID);
    void onError(std::exception_ptr, uint64_t correlationID);

private:
    TileLoader<RasterDEMTile> loader;

    std::shared_ptr<Mailbox> mailbox;
    Actor<RasterDEMTileWorker> worker;
    
    uint64_t correlationID = 0;
    Tileset::DEMEncoding encoding;

    // Contains the Bucket object for the tile. Buckets are render
    // objects and they get added by tile parsing operations.
    std::unique_ptr<HillshadeBucket> bucket;

};

} // namespace mbgl
