#ifndef LLMR_RENDERER_BUCKET
#define LLMR_RENDERER_BUCKET

#include <string>
#include <llmr/map/tile.hpp>
#include <llmr/util/noncopyable.hpp>

namespace llmr {

class Painter;

class Bucket : private util::noncopyable {
public:
    virtual void render(Painter& painter, const std::string& layer_name, const Tile::ID& id) = 0;
    virtual ~Bucket() {}
};

}

#endif
