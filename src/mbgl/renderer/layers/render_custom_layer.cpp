#include <mbgl/renderer/layers/render_custom_layer.hpp>
#include <mbgl/renderer/painter.hpp>
#include <mbgl/renderer/paint_parameters.hpp>
#include <mbgl/style/layers/custom_layer_impl.hpp>
#include <mbgl/map/transform_state.hpp>
#include <mbgl/map/backend_scope.hpp>

namespace mbgl {

using namespace style;

RenderCustomLayer::RenderCustomLayer(Immutable<style::CustomLayer::Impl> _impl)
    : RenderLayer(LayerType::Custom, _impl) {
}

RenderCustomLayer::~RenderCustomLayer() {
    assert(BackendScope::exists());
    if (initialized && impl().deinitializeFn) {
        impl().deinitializeFn(impl().context);
    }
}

const CustomLayer::Impl& RenderCustomLayer::impl() const {
    return static_cast<const CustomLayer::Impl&>(*baseImpl);
}

void RenderCustomLayer::evaluate(const PropertyEvaluationParameters&) {
    passes = RenderPass::Translucent;
}

bool RenderCustomLayer::hasTransition() const {
    return false;
}

std::unique_ptr<Bucket> RenderCustomLayer::createBucket(const BucketParameters&, const std::vector<const RenderLayer*>&) const {
    assert(false);
    return nullptr;
}

void RenderCustomLayer::render(Painter& painter, PaintParameters& paintParameters, RenderSource*) {
    if (!initialized) {
        assert(impl().initializeFn);
        impl().initializeFn(impl().context);
        initialized = true;
    }

    gl::Context& context = painter.context;
    const TransformState& state = painter.state;

    // Reset GL state to a known state so the CustomLayer always has a clean slate.
    context.bindVertexArray = 0;
    context.setDepthMode(painter.depthModeForSublayer(0, gl::DepthMode::ReadOnly));
    context.setStencilMode(gl::StencilMode::disabled());
    context.setColorMode(painter.colorModeForRenderPass());

    CustomLayerRenderParameters parameters;

    parameters.width = state.getSize().width;
    parameters.height = state.getSize().height;
    parameters.latitude = state.getLatLng().latitude();
    parameters.longitude = state.getLatLng().longitude();
    parameters.zoom = state.getZoom();
    parameters.bearing = -state.getAngle() * util::RAD2DEG;
    parameters.pitch = state.getPitch();
    parameters.fieldOfView = state.getFieldOfView();

    assert(impl().renderFn);
    impl().renderFn(impl().context, parameters);

    // Reset the view back to our original one, just in case the CustomLayer changed
    // the viewport or Framebuffer.
    paintParameters.view.bind();
    context.setDirtyState();
}

} // namespace mbgl
