#ifndef MBGL_RENDERER_BUCKET
#define MBGL_RENDERER_BUCKET

#include <string>
#include <memory>
#include <mbgl/map/tile.hpp>
#include <mbgl/util/noncopyable.hpp>

namespace mbgl {

class Painter;
class StyleLayer;

class Bucket : private util::noncopyable {
public:
    virtual void render(Painter& painter, std::shared_ptr<StyleLayer> layer_desc, const Tile::ID& id, const mat4 &matrix) = 0;
    virtual bool hasData() const = 0;
    virtual ~Bucket() {}

};

}

#endif
