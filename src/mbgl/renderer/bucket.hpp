#pragma once

#include <mbgl/renderer/render_pass.hpp>
#include <mbgl/util/noncopyable.hpp>
#include <mbgl/tile/geometry_tile_data.hpp>
#include <mbgl/renderer/render_layer.hpp>

#include <atomic>
#include <string>
#include <unordered_map>

namespace mbgl {

class Painter;
class PaintParameters;
class RenderTile;

namespace gl {
class Context;
} // namespace gl

namespace style {
class Layer;
} // namespace style

class Bucket : private util::noncopyable {
public:
    Bucket() = default;
    virtual ~Bucket() = default;

    // Feature geometries are also used to populate the feature index.
    // Obtaining these is a costly operation, so we do it only once, and
    // pass-by-const-ref the geometries as a second parameter.
    virtual void addFeature(const GeometryTileFeature&,
                            const GeometryCollection&) {};

    // As long as this bucket has a Prepare render pass, this function is getting called. Typically,
    // this only happens once when the bucket is being rendered for the first time.
    virtual void upload(gl::Context&) = 0;

    // Every time this bucket is getting rendered, this function is called. This happens either
    // once or twice (for Opaque and Transparent render passes).
    virtual void render(Painter&, PaintParameters&, const RenderLayer&, const RenderTile&) = 0;

    virtual bool hasData() const = 0;

    virtual float getQueryRadius(const RenderLayer&) const {
        return 0;
    };

    bool needsUpload() const {
        return hasData() && !uploaded;
    }

protected:
    std::atomic<bool> uploaded { false };
};

} // namespace mbgl
