#include <mbgl/platform/default/headless_view.hpp>
#include <mbgl/platform/default/headless_display.hpp>
#include <mbgl/gl/types.hpp>
#include <mbgl/platform/log.hpp>

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
    GLuint bufferId;

    MBGL_CHECK_ERROR(glGenBuffers(1, &bufferId));
    MBGL_CHECK_ERROR(glBindBuffer(GL_PIXEL_PACK_BUFFER, bufferId));
    MBGL_CHECK_ERROR(glBufferData(GL_PIXEL_PACK_BUFFER, size[1] * image.stride(), 0, GL_STREAM_READ));

    // Create a fence just before glReadPixels so we can delay mapping
    // the buffer until all prior GL calls have completed.
    GLsync fence = MBGL_CHECK_ERROR(glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0));

    MBGL_CHECK_ERROR(glReadPixels(0, 0, size[0], size[1], GL_RGBA, GL_UNSIGNED_BYTE, 0));

    // Timeout measued in nanoseconds, 100ms
    GLenum syncStatus = MBGL_CHECK_ERROR(glClientWaitSync(fence, 0, 100000000));

    switch (syncStatus) {
        case GL_ALREADY_SIGNALED:
            Log::Debug(Event::General, "GL_ALREADY_SIGNALED");
            break;
        case GL_CONDITION_SATISFIED:
            Log::Debug(Event::General, "GL_CONDITION_SATISFIED");
            break;
        case GL_TIMEOUT_EXPIRED:
            Log::Warning(Event::General, "GL_TIMEOUT_EXPIRED");
            break;
        case GL_WAIT_FAILED:
            Log::Warning(Event::General, "GL_WAIT_FAILED");
            break;
        default:
            Log::Warning(Event::General, "GL_WAIT_UNKNOWN");
            break;
    }

    MBGL_CHECK_ERROR(glDeleteSync(fence));

    // Map the PBO to process its data by CPU
    GLubyte* ptr = (GLubyte*)glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY);

    if (ptr) {
        const int stride = image.stride();
        uint8_t* rgba = image.data.get();

        for (unsigned int i = 0, j = size[1] - 1; i < j; i++, j--) {
            std::memcpy(rgba + i * stride, ptr + j * stride, stride);
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
