#ifndef LLMR_GEOMETRY_STROKEBUCKET
#define LLMR_GEOMETRY_STROKEBUCKET

#include "bucket.hpp"

#include <string>

namespace llmr {

class Style;
class StrokeBuffer;

class StrokeBucket : public Bucket {
public:
    virtual void draw(const Style& style, const std::string& layerName);

private:
    std::shared_ptr<StrokeBuffer> buffer;

    // hold information on where the vertices are located in the StrokeBuffer
    uint32_t vertex_start;
    uint32_t length;
};

}

#endif
