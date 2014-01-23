#ifndef LLMR_GEOMETRY_BUCKET
#define LLMR_GEOMETRY_BUCKET

#include <string>

namespace llmr {

class Painter;

class Bucket {
public:
    Bucket() = default;
    virtual void render(Painter& painter, const std::string& layer_name) = 0;

private:
    // Make noncopyable
    Bucket(const Bucket&) = delete;
    Bucket(const Bucket&&) = delete;
    Bucket& operator=(const Bucket&) = delete;
    Bucket& operator=(const Bucket && ) = delete;
};

}

#endif
