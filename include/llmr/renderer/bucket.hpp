#ifndef LLMR_RENDERER_BUCKET
#define LLMR_RENDERER_BUCKET

#include <string>
#include <llmr/map/tile.hpp>

namespace llmr {

class Painter;

class Bucket {
public:
    Bucket() = default;
    virtual void render(Painter& painter, const std::string& layer_name, const Tile::ID& id) = 0;

private:
    // Make noncopyable
    Bucket(const Bucket&) = delete;
    Bucket(const Bucket&&) = delete;
    Bucket& operator=(const Bucket&) = delete;
    Bucket& operator=(const Bucket && ) = delete;
};

}

#endif
