#include <mbgl/layer/custom_layer.hpp>
#include <mbgl/renderer/bucket.hpp>
#include <mbgl/map/transform_state.hpp>

namespace mbgl {

CustomLayer::CustomLayer(const std::string& id_,
                         CustomLayerInitializeFunction initializeFn_,
                         CustomLayerRenderFunction renderFn_,
                         CustomLayerDeinitializeFunction deinitializeFn_,
                         void* context_)
    : StyleLayer(Type::Custom) {
    id = id_;
    initializeFn = initializeFn_;
    renderFn = renderFn_;
    deinitializeFn = deinitializeFn_;
    context = context_;
}

CustomLayer::CustomLayer(const CustomLayer& other)
    : StyleLayer(other) {
    id = other.id;
    // Don't copy anything else.
}

CustomLayer::~CustomLayer() {
    if (deinitializeFn) {
        deinitializeFn(context);
    }
}

void CustomLayer::initialize() {
    assert(initializeFn);
    initializeFn(context);
}

void CustomLayer::render(const TransformState& state) const {
    assert(renderFn);

    CustomLayerRenderParameters parameters;

    parameters.width = state.getWidth();
    parameters.height = state.getHeight();
    parameters.latitude = state.getLatLng().latitude;
    parameters.longitude = state.getLatLng().longitude;
    parameters.zoom = state.getZoom();
    parameters.bearing = -state.getAngle() * util::RAD2DEG;
    parameters.pitch = state.getPitch();
    parameters.altitude = state.getAltitude();
    
    mat4 projMatrix;
    mat4 nativeMatrix;
    mat4 extrudeMatrix;
    
    state.getProjMatrix(projMatrix);
    
    // The extrusion matrix.
    matrix::ortho(extrudeMatrix, 0, state.getWidth(), state.getHeight(), 0, 0, -1);
    
    // The native matrix is a 1:1 matrix that paints the coordinates at the
    // same screen position as the vertex specifies.
    matrix::identity(nativeMatrix);
    matrix::multiply(nativeMatrix, projMatrix, nativeMatrix);

    for(int i = 0; i < 16; i++) {
        parameters.projMatrix[i] = projMatrix[i];
        parameters.nativeMatrix[i] = nativeMatrix[i];
        parameters.extrudeMatrix[i] = extrudeMatrix[i];
    }
    
    renderFn(context, parameters);
}

bool CustomLayer::recalculate(const StyleCalculationParameters&) {
    passes = RenderPass::Translucent;
    return false;
}

std::unique_ptr<StyleLayer> CustomLayer::clone() const {
    return std::make_unique<CustomLayer>(*this);
}

std::unique_ptr<Bucket> CustomLayer::createBucket(StyleBucketParameters&) const {
    return nullptr;
}

} // namespace mbgl
