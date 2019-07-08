#include <mbgl/renderer/layers/render_custom_layer.hpp>
#include <mbgl/renderer/paint_parameters.hpp>
#include <mbgl/gfx/renderer_backend.hpp>
#include <mbgl/gfx/backend_scope.hpp>
#include <mbgl/renderer/bucket.hpp>
#include <mbgl/platform/gl_functions.hpp>
#include <mbgl/style/layers/custom_layer_impl.hpp>
#include <mbgl/map/transform_state.hpp>
#include <mbgl/gl/context.hpp>
#include <mbgl/gl/renderable_resource.hpp>
#include <mbgl/util/mat4.hpp>

namespace mbgl {

using namespace style;

inline const CustomLayer::Impl& impl(const Immutable<style::Layer::Impl>& impl) {
    return static_cast<const CustomLayer::Impl&>(*impl);
}

RenderCustomLayer::RenderCustomLayer(Immutable<style::CustomLayer::Impl> _impl)
    : RenderLayer(makeMutable<CustomLayerProperties>(std::move(_impl))),
      host(impl(baseImpl).host) {
    assert(gfx::BackendScope::exists());
    MBGL_CHECK_ERROR(host->initialize());
}

RenderCustomLayer::~RenderCustomLayer() {
    assert(gfx::BackendScope::exists());
    if (contextDestroyed) {
        host->contextLost();
    } else {
        MBGL_CHECK_ERROR(host->deinitialize());
    }
}

LayerRenderer RenderCustomLayer::createRenderer() {
    if (host != impl(baseImpl).host) {
        //If the context changed, deinitialize the previous one before initializing the new one.
        if (host && !contextDestroyed) {
            MBGL_CHECK_ERROR(host->deinitialize());
        }
        host = impl(baseImpl).host;
        MBGL_CHECK_ERROR(host->initialize());
    }

    return [host = this->host](PaintParameters& paintParameters, const LayerRenderItem&) {
        // TODO: remove cast
        auto& glContext = static_cast<gl::Context&>(paintParameters.context);
        const TransformState& state = paintParameters.state;

        // Reset GL state to a known state so the CustomLayer always has a clean slate.
        glContext.bindVertexArray = 0;
        glContext.setDepthMode(paintParameters.depthModeForSublayer(0, gfx::DepthMaskType::ReadOnly));
        glContext.setStencilMode(gfx::StencilMode::disabled());
        glContext.setColorMode(paintParameters.colorModeForRenderPass());
        glContext.setCullFaceMode(gfx::CullFaceMode::disabled());

        CustomLayerRenderParameters parameters;

        parameters.width = state.getSize().width;
        parameters.height = state.getSize().height;
        parameters.latitude = state.getLatLng().latitude();
        parameters.longitude = state.getLatLng().longitude();
        parameters.zoom = state.getZoom();
        parameters.bearing = -state.getBearing() * util::RAD2DEG;
        parameters.pitch = state.getPitch();
        parameters.fieldOfView = state.getFieldOfView();
        mat4 projMatrix;
        state.getProjMatrix(projMatrix);
        parameters.projectionMatrix = projMatrix;

        MBGL_CHECK_ERROR(host->render(parameters));

        // Reset the view back to our original one, just in case the CustomLayer changed
        // the viewport or Framebuffer.
        paintParameters.backend.getDefaultRenderable().getResource<gl::RenderableResource>().bind();
        glContext.setDirtyState();
    };
}

void RenderCustomLayer::evaluate(const PropertyEvaluationParameters&) {
    passes = RenderPass::Translucent;
    // It is fine to not update `evaluatedProperties`, as `baseImpl` should never be updated for this layer.
}

bool RenderCustomLayer::hasTransition() const {
    return false;
}
bool RenderCustomLayer::hasCrossfade() const {
    return false;
}

void RenderCustomLayer::markContextDestroyed() {
    contextDestroyed = true;
}

void RenderCustomLayer::prepare(const LayerPrepareParameters&) {
}

} // namespace mbgl
