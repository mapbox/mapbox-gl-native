#pragma once

#include <cstdint>
#include <exception>

namespace mbgl {

class MapObserver {
public:
    static MapObserver& nullObserver() {
        static MapObserver mapObserver;
        return mapObserver;
    }

    enum class CameraChangeMode : uint32_t {
        Immediate,
        Animated
    };

    enum class RenderMode : uint32_t {
        Partial,
        Full
    };

    virtual void onCameraWillChange(CameraChangeMode) {}
    virtual void onCameraIsChanging() {}
    virtual void onCameraDidChange(CameraChangeMode) {}
    virtual void onWillStartLoadingMap() {}
    virtual void onDidFinishLoadingMap() {}
    virtual void onDidFailLoadingMap(std::exception_ptr) {}
    virtual void onWillStartRenderingFrame() {}
    virtual void onDidFinishRenderingFrame(RenderMode) {}
    virtual void onWillStartRenderingMap() {}
    virtual void onDidFinishRenderingMap(RenderMode) {}
    virtual void onDidFinishLoadingStyle() {}
    virtual void onSourceDidChange() {}
};

} // namespace mbgl
