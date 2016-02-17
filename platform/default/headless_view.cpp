#include <mbgl/platform/default/headless_view.hpp>
#include <mbgl/platform/default/headless_display.hpp>

#include <cassert>
#include <cstring>

namespace mbgl {

HeadlessView::HeadlessView(float pixelRatio_, uint16_t width, uint16_t height)
    : display(std::make_shared<HeadlessDisplay>()), pixelRatio(pixelRatio_) {
    resize(width, height);
}

HeadlessView::HeadlessView(std::shared_ptr<HeadlessDisplay> display_,
                           float pixelRatio_,
                           uint16_t width,
                           uint16_t height)
    : display(std::move(display_)), pixelRatio(pixelRatio_) {
    resize(width, height);
}

HeadlessView::~HeadlessView() {
    activate();
    clearBuffers();
    deactivate();

    destroyContext();
}


void HeadlessView::loadExtensions() {
    if (extensionsLoaded) {
        return;
    }

    gl::InitializeExtensions(initializeExtension);

    extensionsLoaded = true;
}


bool HeadlessView::isActive() const {
    return std::this_thread::get_id() == thread;
}

void HeadlessView::resize(const uint16_t width, const uint16_t height) {
    if(dimensions[0] == width &&
       dimensions[1] == height) {
        return;
    }
    dimensions = {{ width, height }};
    needsResize = true;
}

PremultipliedImage HeadlessView::readStillImage() {
    assert(isActive());

    const unsigned int w = dimensions[0] * pixelRatio;
    const unsigned int h = dimensions[1] * pixelRatio;

    PremultipliedImage image { w, h };
    MBGL_CHECK_ERROR(glReadPixels(0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, image.data.get()));

    const auto stride = image.stride();
    auto tmp = std::make_unique<uint8_t[]>(stride);
    uint8_t* rgba = image.data.get();
    for (int i = 0, j = h - 1; i < j; i++, j--) {
        std::memcpy(tmp.get(), rgba + i * stride, stride);
        std::memcpy(rgba + i * stride, rgba + j * stride, stride);
        std::memcpy(rgba + j * stride, tmp.get(), stride);
    }

    return image;
}
void HeadlessView::notify() {
    // no-op
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
    if (thread != std::thread::id()) {
        throw std::runtime_error("OpenGL context was already current");
    }
    thread = std::this_thread::get_id();

    if (!glContext) {
        if (!display) {
            throw std::runtime_error("Display is not set");
        }
        createContext();
    }

    activateContext();
    loadExtensions();
}

void HeadlessView::deactivate() {
    if (thread == std::thread::id()) {
        throw std::runtime_error("OpenGL context was not current");
    }
    thread = std::thread::id();

    deactivateContext();
}

void HeadlessView::invalidate() {
    // no-op
}

void HeadlessView::beforeRender() {
    if (needsResize) {
        clearBuffers();
        resizeFramebuffer();
        needsResize = false;
    }
}

void HeadlessView::afterRender() {
    // no-op
}

} // namespace mbgl
