#pragma once

#include <mbgl/map/camera.hpp>
#include <mbgl/util/geo.hpp>

namespace mbgl {

class UpdateParameters;

/**
 * RendererState acts as a Memento object that is able to extract data out of UpdateParameters (an opaque object
 * from the public API perspective).
 */
class RendererState {
public:

    // Obtains a CameraOptions out of a given UpdateParameters object.
    static CameraOptions getCameraOptions(UpdateParameters&, const EdgeInsets& = {});

    // Returns whether an image with the given ID exists from a given UpdateParamters object.
    static bool hasImage(UpdateParameters&, const std::string& id);

    // Returns whether a layer with the given ID exists from a given UpdateParamters object.
    static bool hasLayer(UpdateParameters&, const std::string& id);

    // Returns whether a source with the given ID exists from a given UpdateParamters object.
    static bool hasSource(UpdateParameters&, const std::string& id);
};

} // namespace mbgl
