#ifndef MBGL_RENDERER_BUCKET
#define MBGL_RENDERER_BUCKET

#include <mbgl/renderer/render_pass.hpp>
#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/mat4.hpp>

namespace mbgl {

class Painter;
class StyleLayer;
class TileID;

class Bucket : private util::noncopyable {
public:
    // As long as this bucket has a Prepare render pass, this function is getting called. Typically,
    // this only happens once when the bucket is being rendered for the first time.
    virtual void upload() = 0;

    // Every time this bucket is getting rendered, this function is called. This happens either
    // once or twice (for Opaque and Transparent render passes).
    virtual void render(Painter&, const StyleLayer&, const TileID&, const mat4&) = 0;

    virtual ~Bucket() {}

    inline bool needsUpload() const {
        return !uploaded;
    }

protected:
    bool uploaded = false;

};

}

#endif
