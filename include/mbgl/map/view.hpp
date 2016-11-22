#pragma once

#include <mbgl/map/change.hpp>
#include <mbgl/util/chrono.hpp>
#include <mbgl/util/image.hpp>

#include <array>
#include <functional>
#include <memory>

namespace mbgl {

class Map;

class View {
public:
    virtual ~View() = default;

    // Called directly after initialization. Must always return the same value, i.e. it may
    // not change over time.
    virtual float getPixelRatio() const = 0;

    // Called when the View signaled a dimension change. Must return the logical dimension
    // of this map in pixels.
    virtual std::array<uint16_t, 2> getSize() const = 0;

    // Called for every frame that is being rendered. Must return the absolute dimensions of
    // the current framebuffer. Typically, this is the logical width scaled by the pixel ratio,
    // but in case the view was moved to display with a different pixel ratio, it can also be
    // different from that rule.
    virtual std::array<uint16_t, 2> getFramebufferSize() const = 0;

    // Called when this View is associated with a Map object.
    virtual void initialize(Map*);

    // Called when the view's GL context needs to be made active or inactive. These are called,
    // as a matched pair, in four situations:
    //
    //   1. When releasing GL resources during Map destruction
    //   2. When calling a CustomLayerInitializeFunction, during Map::addLayer
    //   3. When calling a CustomLayerDeinitializeFunction, during Map::removeLayer
    //   4. When rendering for Map::renderStill
    //
    // They are *not* called for Map::render; it is assumed that the correct context is already
    // activated prior to calling Map::render.
    virtual void activate() = 0;
    virtual void deactivate() = 0;

    // Called when the map needs to be rendered; the view should call Map::render() at some point
    // in the near future. (Not called for Map::renderStill() mode.)
    virtual void invalidate() = 0;

    // Reads the pixel data from the current framebuffer. If your View implementation
    // doesn't support reading from the framebuffer, return a null pointer.
    virtual PremultipliedImage readStillImage(std::array<uint16_t, 2> size = {{ 0, 0 }});

    // Notifies a watcher of map x/y/scale/rotation changes.
    virtual void notifyMapChange(MapChange change);

protected:
    mbgl::Map *map = nullptr;
};
} // namespace mbgl
