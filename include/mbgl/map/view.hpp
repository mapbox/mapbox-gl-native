#pragma once

#include <mbgl/util/image.hpp>

#include <array>

namespace mbgl {

class Map;

class View : private util::noncopyable {
public:
    virtual ~View() = default;

    // Called when this View is associated with a Map object.
    virtual void initialize(Map*);

    // Called when this View is used for rendering. Implementations should ensure that a renderable
    // object is bound and glClear/glDraw* calls can be done.
    virtual void bind() = 0;

    // Called when the View signaled a dimension change. Must return the logical dimension
    // of this map in pixels.
    virtual std::array<uint16_t, 2> getSize() const = 0;

    // Called for every frame that is being rendered. Must return the absolute dimensions of
    // the current framebuffer. Typically, this is the logical width scaled by the pixel ratio,
    // but in case the view was moved to display with a different pixel ratio, it can also be
    // different from that rule.
    virtual std::array<uint16_t, 2> getFramebufferSize() const = 0;

    // Reads the pixel data from the current framebuffer. If your View implementation
    // doesn't support reading from the framebuffer, return a null pointer.
    virtual PremultipliedImage readStillImage(std::array<uint16_t, 2> size = {{ 0, 0 }});

protected:
    mbgl::Map *map = nullptr;
};

} // namespace mbgl
