#ifndef LLMR_GEOMETRY_BUCKET
#define LLMR_GEOMETRY_BUCKET

#include <string>

namespace llmr {

class Style;
class Painter;

class Bucket {
public:
    virtual void draw(const Style& style, const std::string& layerName) = 0;
    virtual void render(Painter& painter) = 0;
};

}

#endif
