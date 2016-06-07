#pragma once

#include <mbgl/util/noncopyable.hpp>

namespace mbgl {

namespace gl {class ObjectStore; }

class Renderable : private util::noncopyable  {
public:
    virtual void upload(gl::ObjectStore&) = 0;

protected:
    bool uploaded = false;
};

} // namespace mbgl
