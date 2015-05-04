#ifndef MBGL_RENDERER_DEBUGBUCKET
#define MBGL_RENDERER_DEBUGBUCKET

#include <mbgl/renderer/bucket.hpp>
#include <mbgl/geometry/debug_font_buffer.hpp>
#include <mbgl/geometry/vao.hpp>

#include <vector>

namespace mbgl {

class PlainShader;

class DebugBucket : public Bucket {
public:
    DebugBucket(DebugFontBuffer& fontBuffer);

    void upload() override;
    void render(Painter&, const StyleLayer&, const TileID&, const mat4&) override;

    void drawLines(PlainShader& shader);
    void drawPoints(PlainShader& shader);

private:
    DebugFontBuffer& fontBuffer;
    VertexArrayObject array;
};

}

#endif
