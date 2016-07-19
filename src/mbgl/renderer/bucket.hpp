#pragma once

#include <mbgl/gl/gl.hpp>
#include <mbgl/renderer/render_pass.hpp>
#include <mbgl/util/noncopyable.hpp>

#include <atomic>

#define BUFFER_OFFSET_0  ((GLbyte*)nullptr)
#define BUFFER_OFFSET(i) ((BUFFER_OFFSET_0) + (i))

namespace mbgl {

class Painter;
class PaintParameters;
class CollisionTile;
class RenderTile;

namespace gl {
class ObjectStore;
class Config;
} // namespace gl

namespace style {
class Layer;
} // namespace style

class Bucket : private util::noncopyable {
public:
    Bucket() = default;

    // As long as this bucket has a Prepare render pass, this function is getting called. Typically,
    // this only happens once when the bucket is being rendered for the first time.
    virtual void upload(gl::ObjectStore&, gl::Config&) = 0;

    // Every time this bucket is getting rendered, this function is called. This happens either
    // once or twice (for Opaque and Transparent render passes).
    virtual void render(Painter&, PaintParameters&, const style::Layer&, const RenderTile&) = 0;

    virtual ~Bucket() = default;

    virtual bool hasData() const = 0;

    virtual bool needsClipping() const = 0;

    bool needsUpload() const {
        return !uploaded;
    }

    virtual void placeFeatures(CollisionTile&) {}
    virtual void swapRenderData() {}

protected:
    std::atomic<bool> uploaded { false };
};

} // namespace mbgl
