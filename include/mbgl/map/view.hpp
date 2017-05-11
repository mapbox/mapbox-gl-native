#pragma once

namespace mbgl {

class Map;

class View {
public:
    virtual ~View() = default;

    // Called when this View is used for rendering. Implementations should ensure that a renderable
    // object is bound and glClear/glDraw* calls can be done. They should also make sure that
    // calling .bind() repeatedly is a no-op and that the appropriate gl::Context values are
    // set to the current state.
    virtual void bind() = 0;
};

} // namespace mbgl
