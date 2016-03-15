#ifndef MBGL_RENDERER_BUCKET
#define MBGL_RENDERER_BUCKET

#include <mbgl/gl/gl.hpp>
#include <mbgl/renderer/render_pass.hpp>
#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/mat4.hpp>

#include <atomic>

#define BUFFER_OFFSET_0  ((GLbyte*)nullptr)
#define BUFFER_OFFSET(i) ((BUFFER_OFFSET_0) + (i))

namespace mbgl {

class Painter;
class StyleLayer;
class TileID;
class CollisionTile;

namespace gl {
class GLObjectStore;
}

class Bucket : private util::noncopyable {
public:
    Bucket() : uploaded(false) {}

    // As long as this bucket has a Prepare render pass, this function is getting called. Typically,
    // this only happens once when the bucket is being rendered for the first time.
    virtual void upload(gl::GLObjectStore&) = 0;

    // Every time this bucket is getting rendered, this function is called. This happens either
    // once or twice (for Opaque and Transparent render passes).
    virtual void render(Painter&, const StyleLayer&, const TileID&, const mat4&) = 0;

    virtual ~Bucket() = default;

    virtual bool hasData() const = 0;

    virtual bool needsClipping() const = 0;

    inline bool needsUpload() const {
        return !uploaded;
    }

    virtual void placeFeatures(CollisionTile&) {}
    virtual void swapRenderData() {}

protected:
    std::atomic<bool> uploaded;

};

} // namespace mbgl

#endif
