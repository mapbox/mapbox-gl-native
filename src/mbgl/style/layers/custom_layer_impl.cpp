#include <mbgl/style/layers/custom_layer_impl.hpp>
#include <mbgl/renderer/bucket.hpp>
#include <mbgl/map/transform_state.hpp>

namespace mbgl {
namespace style {

CustomLayer::Impl::Impl(const std::string& id_,
                         CustomLayerInitializeFunction initializeFn_,
                         CustomLayerRenderFunction renderFn_,
                         CustomLayerDeinitializeFunction deinitializeFn_,
                         void* context_) {
    id = id_;
    initializeFn = initializeFn_;
    renderFn = renderFn_;
    deinitializeFn = deinitializeFn_;
    context = context_;
}

CustomLayer::Impl::Impl(const CustomLayer::Impl& other)
    : Layer::Impl(other) {
    id = other.id;
    // Don't copy anything else.
}

CustomLayer::Impl::~Impl() = default;

std::unique_ptr<Layer> CustomLayer::Impl::clone() const {
    return std::make_unique<CustomLayer>(*this);
}

std::unique_ptr<Layer> CustomLayer::Impl::cloneRef(const std::string&) const {
    assert(false);
    return std::make_unique<CustomLayer>(*this);
}

void CustomLayer::Impl::stringifyLayout(rapidjson::Writer<rapidjson::StringBuffer>&) const {
}

void CustomLayer::Impl::initialize() {
    assert(initializeFn);
    initializeFn(context);
}

void CustomLayer::Impl::deinitialize() {
    if (deinitializeFn) {
        deinitializeFn(context);
    }
}

void CustomLayer::Impl::render(const TransformState& state) const {
    assert(renderFn);

    CustomLayerRenderParameters parameters;

    parameters.width = state.getSize().width;
    parameters.height = state.getSize().height;
    parameters.latitude = state.getLatLng().latitude;
    parameters.longitude = state.getLatLng().longitude;
    parameters.zoom = state.getZoom();
    parameters.bearing = -state.getAngle() * util::RAD2DEG;
    parameters.pitch = state.getPitch();
    parameters.fieldOfView = state.getFieldOfView();

    renderFn(context, parameters);
}

bool CustomLayer::Impl::evaluate(const PropertyEvaluationParameters&) {
    passes = RenderPass::Translucent;
    return false;
}

std::unique_ptr<Bucket> CustomLayer::Impl::createBucket(BucketParameters&, const GeometryTileLayer&) const {
    return nullptr;
}

} // namespace style
} // namespace mbgl
