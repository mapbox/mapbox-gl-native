#pragma once

#include <mbgl/map/camera.hpp>
#include <mbgl/util/geo.hpp>

namespace mbgl {

class UpdateParameters;

class RendererState {
public:
    static CameraOptions getCameraOptions(UpdateParameters&, const EdgeInsets& = {});
};

} // namespace mbgl
