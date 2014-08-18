#ifndef MBGL_RENDERER_DEBUGBUCKET
#define MBGL_RENDERER_DEBUGBUCKET

#include <mbgl/renderer/bucket.hpp>
#include <mbgl/geometry/debug_font_buffer.hpp>
#include <mbgl/geometry/vao.hpp>

#include <vector>
#include <memory>

#ifndef BUFFER_OFFSET
#define BUFFER_OFFSET(i) ((char *)nullptr + (i))
#endif

namespace mbgl {

class PlainShader;

class DebugBucket : public Bucket {
public:
    DebugBucket(DebugFontBuffer& fontBuffer);

    virtual void render(Painter& painter, std::shared_ptr<StyleLayer> layer_desc, const Tile::ID& id, const mat4 &matrix);
    virtual bool hasData() const;

    void drawLines(PlainShader& shader);
    void drawPoints(PlainShader& shader);

private:
    DebugFontBuffer& fontBuffer;
    VertexArrayObject array;
};

}

#endif
