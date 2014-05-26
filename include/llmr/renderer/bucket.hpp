#ifndef LLMR_RENDERER_BUCKET
#define LLMR_RENDERER_BUCKET

#include <string>
#include <llmr/map/tile.hpp>
#include <llmr/util/noncopyable.hpp>
#include <llmr/renderer/prerendered_texture.hpp>

namespace llmr {

class Painter;

class Bucket : private util::noncopyable {
public:
    virtual void render(Painter& painter, const std::string& layer_name, const Tile::ID& id) = 0;
    virtual bool hasData() const = 0;
    virtual ~Bucket() {}

    std::unique_ptr<PrerenderedTexture> prerendered;
};

}

#endif
