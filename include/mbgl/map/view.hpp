#ifndef MBGL_MAP_VIEW
#define MBGL_MAP_VIEW

#include <chrono>

namespace mbgl {

class Map;

enum MapChange : uint8_t {
    MapChangeRegionWillChange = 0,
    MapChangeRegionWillChangeAnimated = 1,
    MapChangeRegionDidChange = 2,
    MapChangeRegionDidChangeAnimated = 3,
    MapChangeWillStartLoadingMap = 4,
    MapChangeDidFinishLoadingMap = 5,
    MapChangeDidFailLoadingMap = 6,
    MapChangeWillStartRenderingMap = 7,
    MapChangeDidFinishRenderingMap = 8,
    MapChangeDidFinishRenderingMapFullyRendered = 9
};

class View {
public:
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
    // (i.e. map->renderSync() or map->renderAsync() must be called as a result of this)
    virtual void invalidate() = 0;

    // Notifies a watcher of map x/y/scale/rotation changes.
    // Must only be called from the same thread that caused the change.
    // Must not be called from the render thread.
    virtual void notifyMapChange(
        MapChange change,
        std::chrono::steady_clock::duration delay = std::chrono::steady_clock::duration::zero());

protected:
    // Resizes the view
    void resize(uint16_t width, uint16_t height, float ratio, uint16_t fbWidth, uint16_t fbHeight);

protected:
    mbgl::Map *map = nullptr;
};
}

#endif
