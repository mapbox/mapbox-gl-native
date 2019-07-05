#pragma once

#include <mbgl/gfx/texture.hpp>
#include <mbgl/util/image.hpp>
#include <mbgl/util/optional.hpp>

namespace mbgl {

namespace style {
class ColorRampPropertyValue;
} // namespace style

namespace gfx {
class UploadPass;
} // namespace gfx

class ColorRampRenderData {
public:
    explicit ColorRampRenderData(const style::ColorRampPropertyValue&);
    ~ColorRampRenderData();
    void upload(gfx::UploadPass&);
    const PremultipliedImage image;
    optional<gfx::Texture> texture;
};

} // namespace mbgl
