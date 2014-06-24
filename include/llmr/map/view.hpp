#ifndef LLMR_MAP_VIEW
#define LLMR_MAP_VIEW

namespace llmr {

class Map;

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

<<<<<<< Updated upstream
<<<<<<< Updated upstream
    // Notifies a watcher of map x/y/scale/rotation changes.
    // Must only be called from the same thread that caused the change.
    // Must not be called from the render thread.
    virtual void notify_map_change() = 0;

=======
>>>>>>> Stashed changes
=======
>>>>>>> Stashed changes
protected:
    llmr::Map *map = nullptr;
};
}

#endif
