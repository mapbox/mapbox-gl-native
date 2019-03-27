#pragma once

#include <mbgl/style/source.hpp>
#include <mbgl/style/image.hpp>

#include <cstdint>
#include <string>

namespace mbgl {

enum class MapLoadError {
    StyleParseError,
    StyleLoadError,
    NotFoundError,
    UnknownError,
};

class MapObserver {
public:
    virtual ~MapObserver() = default;

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
    virtual void onDidFailLoadingMap(MapLoadError, const std::string&) {}
    virtual void onWillStartRenderingFrame() {}
    virtual void onDidFinishRenderingFrame(RenderMode) {}
    virtual void onWillStartRenderingMap() {}
    virtual void onDidFinishRenderingMap(RenderMode) {}
    virtual void onDidFinishLoadingStyle() {}
    virtual void onSourceChanged(style::Source&) {}
    virtual void onDidBecomeIdle() {}
    virtual void onStyleImageMissing(const std::string&) {}
};

} // namespace mbgl
