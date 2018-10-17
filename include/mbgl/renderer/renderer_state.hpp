#pragma once

#include <mbgl/map/camera.hpp>
#include <mbgl/util/geo.hpp>

namespace mbgl {

class UpdateParameters;

/**
 * RendererState acts as a Memento-like object that is able to extract data out of UpdateParameters (an opaque object
 * from the public API perspective).
 */
class RendererState {
public:

    // Obtains a CameraOptions out of a given UpdateParameters object.
    static CameraOptions getCameraOptions(UpdateParameters&, const EdgeInsets& = {});

};

} // namespace mbgl
