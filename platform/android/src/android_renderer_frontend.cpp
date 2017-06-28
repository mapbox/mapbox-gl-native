#include "android_renderer_frontend.hpp"

#include <mbgl/map/view.hpp>
#include <mbgl/renderer/renderer.hpp>

namespace mbgl {
namespace android {

AndroidRendererFrontend::AndroidRendererFrontend(
        std::unique_ptr<Renderer> renderer_,
        InvalidateCallback invalidate)
        : renderer(std::move(renderer_))
        , asyncInvalidate([=, invalidate=std::move(invalidate)]() {
            invalidate();
        }) {
}

AndroidRendererFrontend::~AndroidRendererFrontend() = default;

void AndroidRendererFrontend::reset() {
    assert (renderer);
    if (renderer) {
        renderer.reset();
    }
}

void AndroidRendererFrontend::setObserver(RendererObserver& observer) {
    assert (renderer);
    renderer->setObserver(&observer);
}

void AndroidRendererFrontend::update(std::shared_ptr<UpdateParameters> params) {
    updateParameters = std::move(params);
    asyncInvalidate.send();
}

void AndroidRendererFrontend::render(View& view) {
    assert (renderer);
    if (!updateParameters) return;

    renderer->render(view, *updateParameters);
}

void AndroidRendererFrontend::onLowMemory() {
    assert (renderer);
    renderer->onLowMemory();
}

std::vector<Feature> AndroidRendererFrontend::querySourceFeatures(const std::string& sourceID,
                                                                  const SourceQueryOptions& options) const {
    return renderer->querySourceFeatures(sourceID, options);
}

std::vector<Feature> AndroidRendererFrontend::queryRenderedFeatures(const ScreenBox& box,
                                                                    const RenderedQueryOptions& options) const {
    return renderer->queryRenderedFeatures(box, options);
}

std::vector<Feature> AndroidRendererFrontend::queryRenderedFeatures(const ScreenCoordinate& point,
                                                                    const RenderedQueryOptions& options) const {
    return renderer->queryRenderedFeatures(point, options);
}

AnnotationIDs AndroidRendererFrontend::queryPointAnnotations(const ScreenBox& box) const {
    return renderer->queryPointAnnotations(box);
}

} // namespace android
} // namespace mbgl

