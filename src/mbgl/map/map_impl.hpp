#pragma once

#include <mbgl/annotation/annotation_manager.hpp>
#include <mbgl/map/map.hpp>
#include <mbgl/map/map_observer.hpp>
#include <mbgl/map/map_options.hpp>
#include <mbgl/map/mode.hpp>
#include <mbgl/map/transform.hpp>
#include <mbgl/renderer/renderer_frontend.hpp>
#include <mbgl/renderer/renderer_observer.hpp>
#include <mbgl/style/observer.hpp>
#include <mbgl/style/source.hpp>
#include <mbgl/style/style.hpp>
#include <mbgl/util/size.hpp>

namespace mbgl {

class FileSource;
class ResourceTransform;

struct StillImageRequest {
    StillImageRequest(Map::StillImageCallback&& callback_)
        : callback(std::move(callback_)) {
    }

    Map::StillImageCallback callback;
};

class Map::Impl : public style::Observer, public RendererObserver {
public:
    Impl(RendererFrontend&, MapObserver&, std::shared_ptr<FileSource>, const MapOptions&);
    ~Impl() final;

    // StyleObserver
    void onSourceChanged(style::Source&) final;
    void onUpdate() final;
    void onStyleLoading() final;
    void onStyleLoaded() final;
    void onStyleError(std::exception_ptr) final;

    // RendererObserver
    void onInvalidate() final;
    void onResourceError(std::exception_ptr) final;
    void onWillStartRenderingFrame() final;
    void onDidFinishRenderingFrame(RenderMode, bool, bool) final;
    void onWillStartRenderingMap() final;
    void onDidFinishRenderingMap() final;
    void onStyleImageMissing(const std::string&, std::function<void()>) final;
    void onRemoveUnusedStyleImages(const std::vector<std::string>&) final;

    // Map
    void jumpTo(const CameraOptions&);

    MapObserver& observer;
    RendererFrontend& rendererFrontend;

    Transform transform;

    const MapMode mode;
    const float pixelRatio;
    const bool crossSourceCollisions;

    MapDebugOptions debugOptions { MapDebugOptions::NoDebug };

    std::shared_ptr<FileSource> fileSource;

    std::unique_ptr<style::Style> style;
    AnnotationManager annotationManager;

    bool cameraMutated = false;

    uint8_t prefetchZoomDelta = util::DEFAULT_PREFETCH_ZOOM_DELTA;

    bool loading = false;
    bool rendererFullyLoaded;
    std::unique_ptr<StillImageRequest> stillImageRequest;
};

} // namespace mbgl
