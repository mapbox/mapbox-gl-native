#ifndef LLMR_MAP_VIEW
#define LLMR_MAP_VIEW

namespace llmr {

class View {
public:
    // Called from the render (=GL) thread. Signals that the context should
    // swap the front and the back buffer.
    virtual void swap() = 0;

    // Called from the render thread. Makes the GL context active in the current
    // thread. This is typically just called once at the beginning of the
    // renderer setup since the render thread doesn't switch the contexts.
    virtual void make_active() = 0;
};
}

#endif
