#ifndef MBGL_MAP_VIEW
#define MBGL_MAP_VIEW

#include <mbgl/util/time.hpp>

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
    virtual void initialize(Map *map) {
        this->map = map;
    }

    // Called from the render (=GL) thread. Signals that the context should
    // swap the front and the back buffer.
    virtual void swap() = 0;

    // Called from the render thread. Makes the GL context active in the current
    // thread. This is typically just called once at the beginning of the
    // renderer setup since the render thread doesn't switch the contexts.
    virtual void make_active() = 0;

    // Called from the render thread. Makes the GL context inactive in the current
    // thread. This is called once just before the rendering thread terminates.
    virtual void make_inactive() = 0;

    virtual void notify() = 0;

    // Notifies a watcher of map x/y/scale/rotation changes.
    // Must only be called from the same thread that caused the change.
    // Must not be called from the render thread.
    virtual void notify_map_change(MapChange change, timestamp delay = 0) = 0;

protected:
    mbgl::Map *map = nullptr;
};
}

#endif
