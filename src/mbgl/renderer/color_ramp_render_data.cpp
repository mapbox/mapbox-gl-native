#include <mbgl/renderer/color_ramp_render_data.hpp>

#include <mbgl/gfx/upload_pass.hpp>
#include <mbgl/style/color_ramp_property_value.hpp>

namespace mbgl {

ColorRampRenderData::ColorRampRenderData(const style::ColorRampPropertyValue& value) 
    : image({256, 1}) {
    assert(!value.isUndefined());
    const auto length = image.bytes();

    for (uint32_t i = 0; i < length; i += 4) {
        const auto color = value.evaluate(static_cast<double>(i) / length);
        image.data[i] = std::floor(color.r * 255);
        image.data[i + 1] = std::floor(color.g * 255);
        image.data[i + 2] = std::floor(color.b * 255);
        image.data[i + 3] = std::floor(color.a * 255);
    }
}

ColorRampRenderData::~ColorRampRenderData() = default;

void ColorRampRenderData::upload(gfx::UploadPass& uploadPass) {
    if (!texture) {
        texture = uploadPass.createTexture(image);
    }
}

} // namespace mbgl
