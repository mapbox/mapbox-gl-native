#pragma once

#include <mbgl/tile/tile_id.hpp>
#include <mbgl/actor/actor_ref.hpp>
#include <mbgl/util/optional.hpp>

#include <memory>
#include <string>

namespace mbgl {

class RasterTile;

namespace style {
class Layer;
} // namespace style

class RasterTileWorker {
public:
    RasterTileWorker(ActorRef<RasterTileWorker>, ActorRef<RasterTile>, OverscaledTileID);
    ~RasterTileWorker();

    void setLayers(std::vector<std::unique_ptr<style::Layer>>);
    void parse(std::shared_ptr<const std::string> data);

private:
    ActorRef<RasterTile> parent;

    const OverscaledTileID id;

    // Outer optional indicates whether we've received it or not.
    optional<std::vector<std::unique_ptr<style::Layer>>> layers;
};

} // namespace mbgl
