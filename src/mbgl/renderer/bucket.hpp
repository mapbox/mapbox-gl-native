#ifndef MBGL_RENDERER_BUCKET
#define MBGL_RENDERER_BUCKET

#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/mat4.hpp>

namespace mbgl {

class Painter;
class StyleLayer;
class TileID;

enum class RenderPass : uint8_t {
    Prepare = 1 << 0,
    Opaque = 1 << 1,
    Translucent = 1 << 2,
};

class Bucket : private util::noncopyable {
public:
    // As long as this bucket has a Prepare render pass, this function is getting called. Typically,
    // this only happens once when the bucket is being rendered for the first time.
    virtual void prepare() = 0;

    // Every time this bucket is getting rendered, this function is called. This happens either
    // once or twice (for Opaque and Transparent render passes).
    virtual void render(Painter&, const StyleLayer&, const TileID&, const mat4&) = 0;

    virtual ~Bucket() {}

    inline bool hasRenderPass(RenderPass pass) const {
        return static_cast<std::underlying_type<RenderPass>::type>(renderPass) &
               static_cast<std::underlying_type<RenderPass>::type>(pass);
    }

protected:
    RenderPass renderPass = RenderPass::Prepare;

};

}

#endif
