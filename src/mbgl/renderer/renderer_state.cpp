#include <mbgl/renderer/renderer_state.hpp>
#include <mbgl/renderer/update_parameters.hpp>

#include <mbgl/style/image_impl.hpp>
#include <mbgl/style/layer_impl.hpp>
#include <mbgl/style/source_impl.hpp>

namespace mbgl {

CameraOptions RendererState::getCameraOptions(UpdateParameters& updateParameters, const EdgeInsets& padding) {
    return updateParameters.transformState.getCameraOptions(padding);
}

bool RendererState::hasImage(UpdateParameters& updateParameters, const std::string& id) {
    for (const auto& image : *updateParameters.images) {
        if (image.get()->id == id) {
            return true;
        }
    }
    return false;
}

bool RendererState::hasLayer(UpdateParameters& updateParameters, const std::string& id) {
    for (const auto& layer : *updateParameters.layers) {
        if (layer.get()->id == id) {
            return true;
        }
    }
    return false;
}

bool RendererState::hasSource(UpdateParameters& updateParameters, const std::string& id) {
    for (const auto& source : *updateParameters.sources) {
        if (source.get()->id == id) {
            return true;
        }
    }
    return false;
}


} // namespace mbgl
