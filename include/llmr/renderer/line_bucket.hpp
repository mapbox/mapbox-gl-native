#ifndef LLMR_RENDERER_LINEBUCKET
#define LLMR_RENDERER_LINEBUCKET

#include "bucket.hpp"

#include <vector>

namespace llmr {

class Style;
class LineBuffer;
struct pbf;

class LineBucket : public Bucket {
public:
    LineBucket(const std::shared_ptr<LineBuffer>& buffer);

    virtual void render(Painter& painter, const std::string& layer_name);

    void addGeometry(pbf& data);

private:
    std::shared_ptr<LineBuffer> buffer;

};

}

#endif
