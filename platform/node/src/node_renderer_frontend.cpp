#include "node_renderer_frontend.hpp"
#include <mbgl/renderer/renderer.hpp>
#include <mbgl/renderer/backend_scope.hpp>

namespace node_mbgl {

NodeRendererFrontend::NodeRendererFrontend(std::unique_ptr<mbgl::Renderer> renderer_, ViewAccessorFunction getView)
    : renderer(std::move(renderer_))
    , asyncInvalidate([&, this, getView] {
        if (renderer && updateParameters) {
            renderer->render(*getView(), *updateParameters);
        }
    }) {
}

NodeRendererFrontend::~NodeRendererFrontend() = default;
    
void NodeRendererFrontend::reset() {
    assert(renderer);
    renderer.reset();
}
    
void NodeRendererFrontend::setObserver(mbgl::RendererObserver& observer_) {
    assert(renderer);
    renderer->setObserver(&observer_);
}

void NodeRendererFrontend::update(std::shared_ptr<mbgl::UpdateParameters> updateParameters_) {
    updateParameters = updateParameters_;
    asyncInvalidate.send();
}
    
std::vector<mbgl::Feature> NodeRendererFrontend::queryRenderedFeatures(const mbgl::ScreenBox& box,
                                                                       const mbgl::RenderedQueryOptions& options) const {
    assert(renderer);
    return renderer->queryRenderedFeatures(box, options);
}

std::vector<mbgl::Feature> NodeRendererFrontend::queryRenderedFeatures(const mbgl::ScreenCoordinate& point,
                                                                       const mbgl::RenderedQueryOptions& options) const {
    assert(renderer);
    return renderer->queryRenderedFeatures(point, options);
}

void NodeRendererFrontend::dumpDebugLogs() {
    assert(renderer);
    return renderer->dumpDebugLogs();
}

} // namespace node_mbgl
