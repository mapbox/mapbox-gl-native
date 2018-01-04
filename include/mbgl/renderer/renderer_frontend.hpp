#pragma once

#include <memory>

namespace mbgl {

class RendererObserver;
class UpdateParameters;

// The RenderFrontend is the bridge between the Map and
// platform used to update and observer the Renderer
//
// It hides any threading specifics and always replies on
// the original thread.
class RendererFrontend {
public:

    virtual ~RendererFrontend() = default;

    // Must synchronously clean up the Renderer if set
    virtual void reset() = 0;

    // Implementer must bind the renderer observer to the renderer in a
    // appropriate manner so that the callbacks occur on the main thread
    virtual void setObserver(RendererObserver&) = 0;

    // Coalescing updates is up to the implementer
    virtual void update(std::shared_ptr<UpdateParameters>) = 0;
};

} // namespace mbgl
