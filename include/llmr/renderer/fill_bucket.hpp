#ifndef LLMR_RENDERER_FILLBUCKET
#define LLMR_RENDERER_FILLBUCKET

#include "bucket.hpp"

#include <vector>

namespace llmr {

class Style;
class FillBuffer;
struct pbf;

class FillBucket : public Bucket {
public:
    FillBucket(const std::shared_ptr<FillBuffer>& buffer);

    virtual void render(Painter& painter, const std::string& layer_name);

    void addGeometry(pbf& data);
    void drawElements(int32_t attrib);
    void drawVertices(int32_t attrib);

private:
    std::shared_ptr<FillBuffer> buffer;

    struct group {
        uint32_t vertex_length;
        uint32_t elements_length;

        group() : vertex_length(0), elements_length(0) {}
        group(uint32_t vertex_length, uint32_t elements_length)
            : vertex_length(vertex_length),
              elements_length(elements_length) {
        }
    };

    // hold information on where the vertices are located in the FillBuffer
    uint32_t vertex_start;
    uint32_t elements_start;
    uint32_t length;
    std::vector<group> groups;
};

}

#endif
