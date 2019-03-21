#include <mbgl/renderer/layers/render_custom_layer.hpp>
#include <mbgl/renderer/paint_parameters.hpp>
#include <mbgl/renderer/backend_scope.hpp>
#include <mbgl/renderer/renderer_backend.hpp>
#include <mbgl/renderer/bucket.hpp>
#include <mbgl/platform/gl_functions.hpp>
#include <mbgl/style/layers/custom_layer_impl.hpp>
#include <mbgl/map/transform_state.hpp>
#include <mbgl/gl/context.hpp>
#include <mbgl/util/mat4.hpp>

namespace mbgl {

using namespace style;

RenderCustomLayer::RenderCustomLayer(Immutable<style::CustomLayer::Impl> _impl)
    : RenderLayer(std::move(_impl)), host(impl().host) {
    assert(BackendScope::exists());
    MBGL_CHECK_ERROR(host->initialize());
}

RenderCustomLayer::~RenderCustomLayer() {
    assert(BackendScope::exists());
    if (contextDestroyed) {
        host->contextLost();
    } else {
        MBGL_CHECK_ERROR(host->deinitialize());
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
bool RenderCustomLayer::hasCrossfade() const {
    return false;
}

void RenderCustomLayer::markContextDestroyed() {
    contextDestroyed = true;
}

void RenderCustomLayer::render(PaintParameters& paintParameters, RenderSource*) {
    if (host != impl().host) {
        //If the context changed, deinitialize the previous one before initializing the new one.
        if (host && !contextDestroyed) {
            MBGL_CHECK_ERROR(host->deinitialize());
        }
        host = impl().host;
        MBGL_CHECK_ERROR(host->initialize());
    }

    // TODO: remove cast
    gl::Context& glContext = reinterpret_cast<gl::Context&>(paintParameters.context);
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
    paintParameters.backend.bind();
    glContext.setDirtyState();
}

} // namespace mbgl
