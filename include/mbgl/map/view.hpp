#ifndef MBGL_MAP_VIEW
#define MBGL_MAP_VIEW

#include <mbgl/util/chrono.hpp>
#include <functional>

#include <memory>

namespace mbgl {

class Map;
class StillImage;

enum MapChange : uint8_t {
    MapChangeRegionWillChange = 0,
    MapChangeRegionWillChangeAnimated = 1,
    MapChangeRegionIsChanging = 2,
    MapChangeRegionDidChange = 3,
    MapChangeRegionDidChangeAnimated = 4,
    MapChangeWillStartLoadingMap = 5,
    MapChangeDidFinishLoadingMap = 6,
    MapChangeDidFailLoadingMap = 7,
    MapChangeWillStartRenderingFrame = 8,
    MapChangeDidFinishRenderingFrame = 9,
    MapChangeDidFinishRenderingFrameFullyRendered = 10,
    MapChangeWillStartRenderingMap = 11,
    MapChangeDidFinishRenderingMap = 12,
    MapChangeDidFinishRenderingMapFullyRendered = 13,
};

class View {
public:
    // Called from the main thread directly after initialization. Must always return the same value,
    // i.e. it may not change over time.
    virtual float getPixelRatio() const = 0;

    // Called from the main thread when the View signaled a dimension change. Must return the
    // logical dimension of this map in pixels.
    virtual std::array<uint16_t, 2> getSize() const = 0;

    // Called from the main thread for every frame that is being rendered. Must return the absolute
    // dimensions of the current framebuffer. Typically, this is the logical width scaled by the
    // pixel ratio, but in case the view was moved to display with a different pixel ratio, it can
    // also be different from that rule.
    virtual std::array<uint16_t, 2> getFramebufferSize() const = 0;

    // Called from the main thread when this View is associated with a Map object.
    virtual void initialize(Map *map_);

    // Called from the render thread. Makes the GL context active in the current
    // thread. This is typically just called once at the beginning of the
    // renderer setup since the render thread doesn't switch the contexts.
    virtual void activate() = 0;

    // Called from the render thread. Makes the GL context inactive in the current
    // thread. This is called once just before the rendering thread terminates.
    virtual void deactivate() = 0;

    virtual void notify() = 0;

    // Called from the render thread. The implementation must trigger a rerender.
    // (map->renderSync() from the main thread must be called as a result of this)
    virtual void invalidate() = 0;

    // Called from the render thread after the render is complete.
    virtual void swap() = 0;

    // Reads the pixel data from the current framebuffer. If your View implementation
    // doesn't support reading from the framebuffer, return a null pointer.
    virtual std::unique_ptr<StillImage> readStillImage();

    // Notifies a watcher of map x/y/scale/rotation changes.
    // Must only be called from the same thread that caused the change.
    // Must not be called from the render thread.
    virtual void notifyMapChange(MapChange change);

protected:
    mbgl::Map *map = nullptr;
};
}

#endif
