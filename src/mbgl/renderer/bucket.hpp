#ifndef MBGL_RENDERER_BUCKET
#define MBGL_RENDERER_BUCKET

#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/mat4.hpp>

namespace mbgl {

class Painter;
class StyleLayer;
class TileID;

class Bucket : private util::noncopyable {
public:
    virtual void render(Painter&, const StyleLayer&, const TileID&, const mat4&) = 0;
    virtual bool hasData() const = 0;
    virtual ~Bucket() {}

};

}

#endif
