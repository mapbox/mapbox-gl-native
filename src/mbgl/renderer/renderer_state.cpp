#include <mbgl/renderer/renderer_state.hpp>
#include <mbgl/renderer/update_parameters.hpp>

namespace mbgl {

CameraOptions RendererState::getCameraOptions(UpdateParameters& updateParameters, const EdgeInsets& padding) {
    return updateParameters.transformState.getCameraOptions(padding);
}

} // namespace mbgl
