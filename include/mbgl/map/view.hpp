#ifndef MBGL_MAP_VIEW
#define MBGL_MAP_VIEW

#include <chrono>
#include <memory>
#include <cassert>

namespace mbgl {

class Map;
class StillImage;

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
    inline virtual void initialize(Map *map_) {
        assert(!map);
        map = map_;
    }

    // Called from the render (=GL) thread. Signals that the contents of the contents
    // may be discarded. The default is a no-op.
    virtual void discard();

    // Called from the render (=GL) thread. Signals that the context should
    // swap the front and the back buffer. The default is a no-op.
    virtual void swap();

    // Called from the render thread. Makes the GL context active in the current
    // thread. This is typically just called once at the beginning of the
    // renderer setup since the render thread doesn't switch the contexts.
    virtual void activate() = 0;

    // Called from the render thread. Makes the GL context inactive in the current
    // thread. This is called once just before the rendering thread terminates.
    virtual void deactivate() = 0;

    // Reads the pixel data from the current framebuffer. If your View implementation
    // doesn't support reading from the framebuffer, return a null pointer.
    virtual std::unique_ptr<StillImage> readStillImage();

    // Notifies a watcher of map x/y/scale/rotation changes.
    // Must only be called from the same thread that caused the change.
    // Must not be called from the render thread.
    // The default is a no-op.
    virtual void notifyMapChange(MapChange change, std::chrono::steady_clock::duration delay = std::chrono::steady_clock::duration::zero());

protected:
    // Resizes the view
    void resize(uint16_t width, uint16_t height, float ratio, uint16_t fbWidth, uint16_t fbHeight);

protected:
    mbgl::Map *map = nullptr;
};
}

#endif
