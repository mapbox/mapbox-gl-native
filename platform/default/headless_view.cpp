#include <mbgl/platform/default/headless_view.hpp>
#include <mbgl/platform/default/headless_display.hpp>

#include <cassert>
#include <cstring>

namespace mbgl {

HeadlessView::HeadlessView(float pixelRatio_, uint16_t width, uint16_t height)
    : display(std::make_shared<HeadlessDisplay>())
    , pixelRatio(pixelRatio_)
    , dimensions({{ width, height }})
    , needsResize(true) {
}

HeadlessView::HeadlessView(std::shared_ptr<HeadlessDisplay> display_,
                           float pixelRatio_,
                           uint16_t width,
                           uint16_t height)
    : display(std::move(display_))
    , pixelRatio(pixelRatio_)
    , dimensions({{ width, height }})
    , needsResize(true) {
}

HeadlessView::~HeadlessView() {
    activate();
    clearBuffers();
    deactivate();

    destroyContext();
}

void HeadlessView::resize(const uint16_t width, const uint16_t height) {
    if(dimensions[0] == width &&
       dimensions[1] == height) {
        return;
    }
    dimensions = {{ width, height }};
    needsResize = true;
}

PremultipliedImage HeadlessView::readStillImage(std::array<uint16_t, 2> size) {
    assert(active);

    if (!size[0] || !size[1]) {
        size[0] = dimensions[0] * pixelRatio;
        size[1] = dimensions[1] * pixelRatio;
    }

    PremultipliedImage image { size[0], size[1] };
    MBGL_CHECK_ERROR(glReadPixels(0, 0, size[0], size[1], GL_RGBA, GL_UNSIGNED_BYTE, image.data.get()));

    const auto stride = image.stride();
    auto tmp = std::make_unique<uint8_t[]>(stride);
    uint8_t* rgba = image.data.get();
    for (int i = 0, j = size[1] - 1; i < j; i++, j--) {
        std::memcpy(tmp.get(), rgba + i * stride, stride);
        std::memcpy(rgba + i * stride, rgba + j * stride, stride);
        std::memcpy(rgba + j * stride, tmp.get(), stride);
    }

    return image;
}

float HeadlessView::getPixelRatio() const {
    return pixelRatio;
}

std::array<uint16_t, 2> HeadlessView::getSize() const {
    return dimensions;
}

std::array<uint16_t, 2> HeadlessView::getFramebufferSize() const {
    return {{ static_cast<uint16_t>(dimensions[0] * pixelRatio),
              static_cast<uint16_t>(dimensions[1] * pixelRatio) }};
}

void HeadlessView::activate() {
    active = true;

    if (!glContext) {
        if (!display) {
            throw std::runtime_error("Display is not set");
        }
        createContext();
    }

    activateContext();

    if (!extensionsLoaded) {
        gl::InitializeExtensions(initializeExtension);
        extensionsLoaded = true;
    }

    if (needsResize) {
        clearBuffers();
        resizeFramebuffer();
        needsResize = false;
    }
}

void HeadlessView::deactivate() {
    deactivateContext();
    active = false;
}

void HeadlessView::invalidate() {
    assert(false);
}

void HeadlessView::notifyMapChange(MapChange change) {
    if (mapChangeCallback) {
        mapChangeCallback(change);
    }
}

} // namespace mbgl
