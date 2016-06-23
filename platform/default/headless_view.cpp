#include <mbgl/platform/default/headless_view.hpp>
#include <mbgl/platform/default/headless_display.hpp>

#include <cassert>
#include <cstring>

namespace {

std::array<uint16_t, 2> dimensionsLimits = {{ 4096, 4096 }};

}

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
    if (dimensions[0] == width && dimensions[1] == height) {
        return;
    }

    if (width > dimensionsLimits[0] || height > dimensionsLimits[1]) {
        return;
    }

    dimensions = {{ width, height }};
    needsResize = true;
}

std::shared_ptr<const PremultipliedImage> HeadlessView::readStillImage() {
    MBGL_VERIFY_THREAD(tid);

    assert(active);
    assert(dimensions[0] <= dimensionsLimits[0]);
    assert(dimensions[1] <= dimensionsLimits[1]);

    static auto image = std::make_shared<PremultipliedImage>(
        dimensionsLimits[0] * pixelRatio, dimensionsLimits[1] * pixelRatio);

    static auto tmp = std::make_unique<uint8_t[]>(image->stride());

    const unsigned int w = dimensions[0] * pixelRatio;
    const unsigned int h = dimensions[1] * pixelRatio;

    MBGL_CHECK_ERROR(glReadPixels(0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, image->data.get()));

    image->height = h;
    image->width = w;

    const auto stride = image->stride();

    uint8_t* rgba = image->data.get();
    for (int i = 0, j = h - 1; i < j; i++, j--) {
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

} // namespace mbgl
