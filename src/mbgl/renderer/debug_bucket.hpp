#ifndef MBGL_RENDERER_DEBUGBUCKET
#define MBGL_RENDERER_DEBUGBUCKET

#include <mbgl/renderer/bucket.hpp>
#include <mbgl/geometry/debug_font_buffer.hpp>
#include <mbgl/geometry/vao.hpp>

#include <vector>

#ifndef BUFFER_OFFSET
#define BUFFER_OFFSET(i) ((char *)nullptr + (i))
#endif

namespace mbgl {

class PlainShader;

class DebugBucket : public Bucket {
public:
    DebugBucket(DebugFontBuffer& fontBuffer);

    void render(Painter &painter, const StyleLayer &layer_desc, const Tile::ID &id,
                const mat4 &matrix) override;
    bool hasData() const override;

    void drawLines(PlainShader& shader);
    void drawPoints(PlainShader& shader);

private:
    DebugFontBuffer& fontBuffer;
    VertexArrayObject array;
};

}

#endif
