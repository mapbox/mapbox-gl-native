#ifndef LLMR_RENDERER_DEBUGBUCKET
#define LLMR_RENDERER_DEBUGBUCKET

#include <llmr/renderer/bucket.hpp>
#include <llmr/style/bucket_description.hpp>
#include <llmr/geometry/debug_font_buffer.hpp>
#include <llmr/geometry/vao.hpp>

#include <vector>
#include <memory>

#ifndef BUFFER_OFFSET
#define BUFFER_OFFSET(i) ((char *)nullptr + (i))
#endif

namespace llmr {

class PlainShader;

class DebugBucket : public Bucket {
public:
    DebugBucket(DebugFontBuffer& fontBuffer);

    virtual void render(Painter& painter, std::shared_ptr<StyleLayer> layer_desc, const Tile::ID& id);
    virtual bool hasData() const;

    void drawLines(PlainShader& shader);
    void drawPoints(PlainShader& shader);

private:
    DebugFontBuffer& fontBuffer;
    VertexArrayObject array;
};

}

#endif
