#pragma once

#include <mbgl/map/view.hpp>
#include <mbgl/gl/types.hpp>

namespace mbgl {

class HeadlessView : public View {
public:
    HeadlessView(float pixelRatio = 1, uint16_t width = 256, uint16_t height = 256);
    ~HeadlessView() override;

    void bind() override;

    std::array<uint16_t, 2> getSize() const override;
    std::array<uint16_t, 2> getFramebufferSize() const override;


    PremultipliedImage readStillImage(std::array<uint16_t, 2> size = {{ 0, 0 }}) override;

    float getPixelRatio() const;

    void resize(uint16_t width, uint16_t height);

private:
    void clearBuffers();
    void resizeFramebuffer();
    void bindFramebuffer();

private:
    const float pixelRatio;
    std::array<uint16_t, 2> dimensions;

    bool needsResize = false;

    gl::FramebufferID fbo = 0;
    gl::RenderbufferID fboDepthStencil = 0;
    gl::RenderbufferID fboColor = 0;
};

} // namespace mbgl
