#include <mbgl/platform/default/headless_display.hpp>
#include <mbgl/platform/default/headless_view.hpp>
#include <mbgl/util/stopwatch.hpp>

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

PremultipliedImage HeadlessView::readStillImage() {
    util::stopwatch stopwatch("readStillImage", EventSeverity::Info, Event::General);
    assert(active);

    const unsigned int w = dimensions[0] * pixelRatio;
    const unsigned int h = dimensions[1] * pixelRatio;

    PremultipliedImage image { w, h };
    GLuint bufferId;

    MBGL_CHECK_ERROR(glGenBuffers(1, &bufferId));
    MBGL_CHECK_ERROR(glBindBuffer(GL_PIXEL_PACK_BUFFER, bufferId));
    MBGL_CHECK_ERROR(glBufferData(GL_PIXEL_PACK_BUFFER, h * image.stride(), 0, GL_STREAM_READ));

    {
        util::stopwatch stopwatch2("glReadPixels", EventSeverity::Info, Event::General);
        MBGL_CHECK_ERROR(glReadPixels(0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, 0));
    }

    // map the PBO to process its data by CPU
    GLubyte* ptr = (GLubyte*)glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY);

    if (ptr) {
        const int stride = image.stride();
        auto tmp = std::make_unique<uint8_t[]>(stride);
        uint8_t* rgba = image.data.get();

        {
            util::stopwatch stopwatch3("memcpy", EventSeverity::Info, Event::General);
            for (unsigned int i = 0, j = h - 1; i < h; i++, j--) {
                std::memcpy(rgba + i * stride, ptr + j * stride, stride);
            }
        }

        MBGL_CHECK_ERROR(glUnmapBuffer(GL_PIXEL_PACK_BUFFER));
    } else {
        throw std::runtime_error(std::string("Could not map buffer on return from read pixels."));
    }

    MBGL_CHECK_ERROR(glDeleteBuffers(1, &bufferId));

    // back to conventional pixel operation
    MBGL_CHECK_ERROR(glBindBuffer(GL_PIXEL_PACK_BUFFER, 0));

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
