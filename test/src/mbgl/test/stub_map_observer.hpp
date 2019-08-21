#pragma once

#include <mbgl/map/map_observer.hpp>

#include <functional>

namespace mbgl {

class StubMapObserver : public MapObserver {
public:
    void onWillStartLoadingMap() final {
        if (willStartLoadingMapCallback) {
            willStartLoadingMapCallback();
        }
    }

    void onDidFinishLoadingMap() final {
        if (didFinishLoadingMapCallback) {
            didFinishLoadingMapCallback();
        }
    }

    void onDidFailLoadingMap(MapLoadError, const std::string&) final {
        if (didFailLoadingMapCallback) {
            didFailLoadingMapCallback();
        }
    }

    void onDidFinishLoadingStyle() final {
        if (didFinishLoadingStyleCallback) {
            didFinishLoadingStyleCallback();
        }
    }

    void onDidFinishRenderingFrame(RenderMode mode, bool) final {
        if (didFinishRenderingFrameCallback) {
            didFinishRenderingFrameCallback(mode);
        }
    }

    std::function<void()> willStartLoadingMapCallback;
    std::function<void()> didFinishLoadingMapCallback;
    std::function<void()> didFailLoadingMapCallback;
    std::function<void()> didFinishLoadingStyleCallback;
    std::function<void(RenderMode)> didFinishRenderingFrameCallback;
};


} // namespace mbgl
