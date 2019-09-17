#include <mbgl/layermanager/layer_manager.hpp>
#include <mbgl/map/map_impl.hpp>
#include <mbgl/renderer/update_parameters.hpp>
#include <mbgl/storage/file_source.hpp>
#include <mbgl/style/style_impl.hpp>
#include <mbgl/util/exception.hpp>

namespace mbgl {

Map::Impl::Impl(RendererFrontend& frontend_,
                MapObserver& observer_,
                std::shared_ptr<FileSource> fileSource_,
                const MapOptions& mapOptions)
        : observer(observer_),
          rendererFrontend(frontend_),
          transform(observer, mapOptions.constrainMode(), mapOptions.viewportMode()),
          mode(mapOptions.mapMode()),
          pixelRatio(mapOptions.pixelRatio()),
          crossSourceCollisions(mapOptions.crossSourceCollisions()),
          fileSource(std::move(fileSource_)),
          style(std::make_unique<style::Style>(*fileSource, pixelRatio)),
          annotationManager(*style) {
    transform.setNorthOrientation(mapOptions.northOrientation());
    style->impl->setObserver(this);
    rendererFrontend.setObserver(*this);
    transform.resize(mapOptions.size());
}

Map::Impl::~Impl() {
    // Explicitly reset the RendererFrontend first to ensure it releases
    // All shared resources (AnnotationManager)
    rendererFrontend.reset();
};

#pragma mark - Map::Impl StyleObserver

void Map::Impl::onSourceChanged(style::Source& source) {
    observer.onSourceChanged(source);
}

void Map::Impl::onUpdate() {
    // Don't load/render anything in still mode until explicitly requested.
    if (mode != MapMode::Continuous && !stillImageRequest) {
        return;
    }

    TimePoint timePoint = mode == MapMode::Continuous ? Clock::now() : Clock::time_point::max();

    transform.updateTransitions(timePoint);

    UpdateParameters params = {
        style->impl->isLoaded(),
        mode,
        pixelRatio,
        debugOptions,
        timePoint,
        transform.getState(),
        style->impl->getGlyphURL(),
        style->impl->spriteLoaded,
        style->impl->getTransitionOptions(),
        style->impl->getLight()->impl,
        style->impl->getImageImpls(),
        style->impl->getSourceImpls(),
        style->impl->getLayerImpls(),
        annotationManager,
        fileSource,
        prefetchZoomDelta,
        bool(stillImageRequest),
        crossSourceCollisions
    };

    rendererFrontend.update(std::make_shared<UpdateParameters>(std::move(params)));
}

void Map::Impl::onStyleLoading() {
    loading = true;
    rendererFullyLoaded = false;
    observer.onWillStartLoadingMap();
}

void Map::Impl::onStyleLoaded() {
    if (!cameraMutated) {
        jumpTo(style->getDefaultCamera());
    }
    if (LayerManager::annotationsEnabled) {
        annotationManager.onStyleLoaded();
    }
    observer.onDidFinishLoadingStyle();
}

void Map::Impl::onStyleError(std::exception_ptr error) {
    MapLoadError type;
    std::string description;

    try {
        std::rethrow_exception(error);
    } catch (const mbgl::util::StyleParseException& e) {
        type = MapLoadError::StyleParseError;
        description = e.what();
    } catch (const mbgl::util::StyleLoadException& e) {
        type = MapLoadError::StyleLoadError;
        description = e.what();
    } catch (const mbgl::util::NotFoundException& e) {
        type = MapLoadError::NotFoundError;
        description = e.what();
    } catch (const std::exception& e) {
        type = MapLoadError::UnknownError;
        description = e.what();
    }

    observer.onDidFailLoadingMap(type, description);
}

#pragma mark - Map::Impl RendererObserver

void Map::Impl::onInvalidate() {
    onUpdate();
}

void Map::Impl::onResourceError(std::exception_ptr error) {
    if (mode != MapMode::Continuous && stillImageRequest) {
        auto request = std::move(stillImageRequest);
        request->callback(error);
    }
}

void Map::Impl::onWillStartRenderingFrame() {
    if (mode == MapMode::Continuous) {
        observer.onWillStartRenderingFrame();
    }
}

void Map::Impl::onDidFinishRenderingFrame(RenderMode renderMode, bool needsRepaint, bool placemenChanged) {
    rendererFullyLoaded = renderMode == RenderMode::Full;

    if (mode == MapMode::Continuous) {
        observer.onDidFinishRenderingFrame({MapObserver::RenderMode(renderMode), needsRepaint, placemenChanged});

        if (needsRepaint || transform.inTransition()) {
            onUpdate();
        } else if (rendererFullyLoaded) {
            observer.onDidBecomeIdle();
        }
    } else if (stillImageRequest && rendererFullyLoaded) {
        auto request = std::move(stillImageRequest);
        request->callback(nullptr);
    }
}

void Map::Impl::onWillStartRenderingMap() {
    if (mode == MapMode::Continuous) {
        observer.onWillStartRenderingMap();
    }
}

void Map::Impl::onDidFinishRenderingMap() {
    if (mode == MapMode::Continuous && loading) {
        observer.onDidFinishRenderingMap(MapObserver::RenderMode::Full);
        if (loading) {
            loading = false;
            observer.onDidFinishLoadingMap();
        }
    }
};

void Map::Impl::jumpTo(const CameraOptions& camera) {
    cameraMutated = true;
    transform.jumpTo(camera);
    onUpdate();
}

void Map::Impl::onStyleImageMissing(const std::string& id, std::function<void()> done) {

    if (style->getImage(id) == nullptr) {
        observer.onStyleImageMissing(id);
    }

    done();
    onUpdate();
}

void Map::Impl::onRemoveUnusedStyleImages(const std::vector<std::string>& unusedImageIDs) {
    for (const auto& unusedImageID : unusedImageIDs) {
        if (observer.onCanRemoveUnusedStyleImage(unusedImageID)) {
            style->removeImage(unusedImageID);
        }
    }
}

} // namespace mbgl
