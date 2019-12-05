#include <mbgl/renderer/renderer_state.hpp>
#include <mbgl/renderer/update_parameters.hpp>

#include <mbgl/style/image_impl.hpp>
#include <mbgl/style/layer_impl.hpp>
#include <mbgl/style/source_impl.hpp>

namespace mbgl {

CameraOptions RendererState::getCameraOptions(const UpdateParameters& updateParameters, const EdgeInsets& padding) {
    return updateParameters.transformState.getCameraOptions(padding);
}

bool RendererState::hasImage(const UpdateParameters& updateParameters, const std::string& id) {
    for (const auto& image : *updateParameters.images) {
        if (image.get()->id == id) {
            return true;
        }
    }
    return false;
}

bool RendererState::hasLayer(const UpdateParameters& updateParameters, const std::string& id) {
    for (const auto& layer : *updateParameters.layers) {
        if (layer.get()->id == id) {
            return true;
        }
    }
    return false;
}

bool RendererState::hasSource(const UpdateParameters& updateParameters, const std::string& id) {
    for (const auto& source : *updateParameters.sources) {
        if (source.get()->id == id) {
            return true;
        }
    }
    return false;
}

ScreenCoordinate RendererState::pixelForLatLng(const UpdateParameters& updateParameters, const LatLng& latLng) {
    // If the center and point longitudes are not in the same side of the
    // antimeridian, we unwrap the point longitude so it would be seen if
    // e.g. the next antimeridian side is visible.
    LatLng unwrappedLatLng = latLng.wrapped();
    unwrappedLatLng.unwrapForShortestPath(updateParameters.transformState.getLatLng());
    const ScreenCoordinate point = updateParameters.transformState.latLngToScreenCoordinate(latLng);
    return ScreenCoordinate{point.x, updateParameters.transformState.getSize().height - point.y};
}

LatLng RendererState::latLngForPixel(const UpdateParameters& updateParameters, const ScreenCoordinate& point) {
    ScreenCoordinate flippedPoint = point;
    flippedPoint.y = updateParameters.transformState.getSize().height - flippedPoint.y;
    return updateParameters.transformState.screenCoordinateToLatLng(flippedPoint);
}

} // namespace mbgl
