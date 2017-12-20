#include <mbgl/renderer/layers/render_custom_layer.hpp>
#include <mbgl/renderer/paint_parameters.hpp>
#include <mbgl/renderer/backend_scope.hpp>
#include <mbgl/renderer/renderer_backend.hpp>
#include <mbgl/renderer/bucket.hpp>
#include <mbgl/style/layers/custom_layer_impl.hpp>
#include <mbgl/map/transform_state.hpp>

namespace mbgl {

using namespace style;

RenderCustomLayer::RenderCustomLayer(Immutable<style::CustomLayer::Impl> _impl)
    : RenderLayer(LayerType::Custom, _impl) {
}

RenderCustomLayer::~RenderCustomLayer() {
    assert(BackendScope::exists());
    if (initialized) {
        if (contextDestroyed && impl().contextLostFn ) {
            impl().contextLostFn(impl().context);
        } else if (!contextDestroyed && impl().deinitializeFn) {
            impl().deinitializeFn(impl().context);
        }
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

void RenderCustomLayer::render(PaintParameters& paintParameters, RenderSource*) {
    if (context != impl().context || !initialized) {
        //If the context changed, deinitialize the previous one before initializing the new one.
        if (context && !contextDestroyed && impl().deinitializeFn) {
            impl().deinitializeFn(context);
        }
        context = impl().context;
        assert(impl().initializeFn);
        impl().initializeFn(impl().context);
        initialized = true;
    }

    gl::Context& glContext = paintParameters.context;
    const TransformState& state = paintParameters.state;

    // Reset GL state to a known state so the CustomLayer always has a clean slate.
    glContext.bindVertexArray = 0;
    glContext.setDepthMode(paintParameters.depthModeForSublayer(0, gl::DepthMode::ReadOnly));
    glContext.setStencilMode(gl::StencilMode::disabled());
    glContext.setColorMode(paintParameters.colorModeForRenderPass());

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
    impl().renderFn(context, parameters);

    // Reset the view back to our original one, just in case the CustomLayer changed
    // the viewport or Framebuffer.
    paintParameters.backend.bind();
    glContext.setDirtyState();
}

} // namespace mbgl
