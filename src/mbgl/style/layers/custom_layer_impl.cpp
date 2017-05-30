#include <mbgl/style/layers/custom_layer_impl.hpp>
#include <mbgl/map/transform_state.hpp>

namespace mbgl {
namespace style {

CustomLayer::Impl::Impl(const std::string& id_,
                         CustomLayerInitializeFunction initializeFn_,
                         CustomLayerRenderFunction renderFn_,
                         CustomLayerDeinitializeFunction deinitializeFn_,
                         void* context_)
    : Layer::Impl(LayerType::Custom, id_, std::string()) {
    initializeFn = initializeFn_;
    renderFn = renderFn_;
    deinitializeFn = deinitializeFn_;
    context = context_;
}

bool CustomLayer::Impl::hasLayoutDifference(const Layer::Impl&) const {
    return false;
}

void CustomLayer::Impl::stringifyLayout(rapidjson::Writer<rapidjson::StringBuffer>&) const {
}

void CustomLayer::Impl::initialize() const {
    assert(initializeFn);
    initializeFn(context);
}

void CustomLayer::Impl::deinitialize() const {
    if (deinitializeFn) {
        deinitializeFn(context);
    }
}

void CustomLayer::Impl::render(const TransformState& state) const {
    assert(renderFn);

    CustomLayerRenderParameters parameters;

    parameters.width = state.getSize().width;
    parameters.height = state.getSize().height;
    parameters.latitude = state.getLatLng().latitude();
    parameters.longitude = state.getLatLng().longitude();
    parameters.zoom = state.getZoom();
    parameters.bearing = -state.getAngle() * util::RAD2DEG;
    parameters.pitch = state.getPitch();
    parameters.fieldOfView = state.getFieldOfView();

    renderFn(context, parameters);
}

} // namespace style
} // namespace mbgl
