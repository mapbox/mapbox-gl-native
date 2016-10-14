#pragma once

#include <mbgl/tile/tile.hpp>
#include <mbgl/tile/tile_loader.hpp>
#include <mbgl/tile/raster_tile_worker.hpp>
#include <mbgl/actor/actor.hpp>

namespace mbgl {

class Tileset;

namespace style {
class Style;
class Layer;
class UpdateParameters;
} // namespace style

class RasterTile : public Tile {
public:
    RasterTile(const OverscaledTileID&,
               std::string sourceID,
               const style::UpdateParameters&,
               const Tileset&);
    ~RasterTile() final;

    void setNecessity(Necessity) final;

    void setError(std::exception_ptr);
    void setData(std::shared_ptr<const std::string> data,
                 optional<Timestamp> modified_,
                 optional<Timestamp> expires_);

    void cancel() override;

    class ParseResult {
    public:
        std::unordered_map<std::string, std::unique_ptr<Bucket>> buckets;
    };

    Bucket* getBucket(const style::Layer&) override;

    void onParsed(ParseResult);
    void onError(std::exception_ptr);

private:
    TileLoader<RasterTile> loader;

    const std::string sourceID;
    style::Style& style;

    std::shared_ptr<Mailbox> mailbox;
    Actor<RasterTileWorker> worker;

    std::unordered_map<std::string, std::unique_ptr<Bucket>> buckets;
};

} // namespace mbgl
