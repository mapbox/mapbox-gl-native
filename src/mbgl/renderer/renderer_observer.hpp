#pragma once

#include <mbgl/util/event.hpp>

#include <exception>

namespace mbgl {

class RendererObserver {
public:
    virtual ~RendererObserver() = default;

    enum class RenderMode : uint32_t {
        Partial,
        Full
    };

    // Signals that a repaint is required
    virtual void onInvalidate() {}

    // Resource failed to download / parse
    virtual void onResourceError(std::exception_ptr, EventSeverity = EventSeverity::Error) {}

    // First frame
    virtual void onWillStartRenderingMap() {}

    // Start of frame, initial is the first frame for this map
    virtual void onWillStartRenderingFrame() {}

    // End of frame, boolean flags that a repaint is required
    virtual void onDidFinishRenderingFrame(RenderMode, bool) {}

    // Final frame
    virtual void onDidFinishRenderingMap() {}
};

} // namespace mbgl
