#include <mbgl/platform/default/glfw_view.hpp>
#include <mbgl/platform/gl.hpp>
#include <mbgl/platform/log.hpp>

pthread_once_t loadGLExtensions = PTHREAD_ONCE_INIT;

GLFWView::GLFWView(bool fullscreen_) : fullscreen(fullscreen_) {
#ifdef NVIDIA
    glDiscardFramebufferEXT = reinterpret_cast<PFNGLDISCARDFRAMEBUFFEREXTPROC>(glfwGetProcAddress("glDiscardFramebufferEXT"));
#endif
}

GLFWView::~GLFWView() {
    glfwDestroyWindow(window);
    glfwTerminate();
}

void glfwError(int error, const char *description) {
    mbgl::Log::Error(mbgl::Event::OpenGL, "GLFW error (%i): %s", error, description);
    assert(false);
}

void GLFWView::initialize(mbgl::Map *map_) {
    View::initialize(map_);

    glfwSetErrorCallback(glfwError);

    if (!glfwInit()) {
        mbgl::Log::Error(mbgl::Event::OpenGL, "failed to initialize glfw");
        exit(1);
    }

    GLFWmonitor *monitor = nullptr;
    if (fullscreen) {
        monitor = glfwGetPrimaryMonitor();
    }

#ifdef DEBUG
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif

#ifdef GL_ES_VERSION_2_0
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
#endif

    glfwWindowHint(GLFW_RED_BITS, 8);
    glfwWindowHint(GLFW_GREEN_BITS, 8);
    glfwWindowHint(GLFW_BLUE_BITS, 8);
    glfwWindowHint(GLFW_ALPHA_BITS, 8);
    glfwWindowHint(GLFW_STENCIL_BITS, 8);
    glfwWindowHint(GLFW_DEPTH_BITS, 16);

    window = glfwCreateWindow(1024, 768, "Mapbox GL", monitor, NULL);
    if (!window) {
        glfwTerminate();
        mbgl::Log::Error(mbgl::Event::OpenGL, "failed to initialize window");
        exit(1);
    }

    glfwSetWindowUserPointer(window, this);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    int width, height;
    glfwGetWindowSize(window, &width, &height);
    onResize(window, width, height);

    glfwSetCursorPosCallback(window, onMouseMove);
    glfwSetMouseButtonCallback(window, onMouseClick);
    glfwSetWindowSizeCallback(window, onResize);
    glfwSetFramebufferSizeCallback(window, onResize);
    glfwSetScrollCallback(window, onScroll);
    glfwSetKeyCallback(window, onKey);

    pthread_once(&loadGLExtensions, [] {
        const std::string extensions = reinterpret_cast<const char *>(MBGL_CHECK_ERROR(glGetString(GL_EXTENSIONS)));
        using namespace mbgl;

        if (extensions.find("GL_KHR_debug") != std::string::npos) {
            gl::DebugMessageControl = reinterpret_cast<gl::PFNGLDEBUGMESSAGECONTROLPROC>(glfwGetProcAddress("glDebugMessageControl"));
            gl::DebugMessageInsert = reinterpret_cast<gl::PFNGLDEBUGMESSAGEINSERTPROC>(glfwGetProcAddress("glDebugMessageInsert"));
            gl::DebugMessageCallback = reinterpret_cast<gl::PFNGLDEBUGMESSAGECALLBACKPROC>(glfwGetProcAddress("glDebugMessageCallback"));
            gl::GetDebugMessageLog = reinterpret_cast<gl::PFNGLGETDEBUGMESSAGELOGPROC>(glfwGetProcAddress("glGetDebugMessageLog"));
            gl::GetPointerv = reinterpret_cast<gl::PFNGLGETPOINTERVPROC>(glfwGetProcAddress("glGetPointerv"));
            gl::PushDebugGroup = reinterpret_cast<gl::PFNGLPUSHDEBUGGROUPPROC>(glfwGetProcAddress("glPushDebugGroup"));
            gl::PopDebugGroup = reinterpret_cast<gl::PFNGLPOPDEBUGGROUPPROC>(glfwGetProcAddress("glPopDebugGroup"));
            gl::ObjectLabel = reinterpret_cast<gl::PFNGLOBJECTLABELPROC>(glfwGetProcAddress("glObjectLabel"));
            gl::GetObjectLabel = reinterpret_cast<gl::PFNGLGETOBJECTLABELPROC>(glfwGetProcAddress("glGetObjectLabel"));
            gl::ObjectPtrLabel = reinterpret_cast<gl::PFNGLOBJECTPTRLABELPROC>(glfwGetProcAddress("glObjectPtrLabel"));
            gl::GetObjectPtrLabel = reinterpret_cast<gl::PFNGLGETOBJECTPTRLABELPROC>(glfwGetProcAddress("glGetObjectPtrLabel"));
            assert(gl::DebugMessageControl != nullptr);
            assert(gl::DebugMessageInsert != nullptr);
            assert(gl::DebugMessageCallback != nullptr);
            assert(gl::GetDebugMessageLog != nullptr);
            assert(gl::GetPointerv != nullptr);
            assert(gl::PushDebugGroup != nullptr);
            assert(gl::PopDebugGroup != nullptr);
            assert(gl::ObjectLabel != nullptr);
            assert(gl::GetObjectLabel != nullptr);
            assert(gl::ObjectPtrLabel != nullptr);
            assert(gl::GetObjectPtrLabel != nullptr);
        } else {
            if (extensions.find("GL_ARB_debug_output") != std::string::npos) {
                gl::DebugMessageControl = reinterpret_cast<gl::PFNGLDEBUGMESSAGECONTROLPROC>(glfwGetProcAddress("glDebugMessageControlARB"));
                gl::DebugMessageInsert = reinterpret_cast<gl::PFNGLDEBUGMESSAGEINSERTPROC>(glfwGetProcAddress("glDebugMessageInsertARB"));
                gl::DebugMessageCallback = reinterpret_cast<gl::PFNGLDEBUGMESSAGECALLBACKPROC>(glfwGetProcAddress("glDebugMessageCallbackARB"));
                gl::GetDebugMessageLog = reinterpret_cast<gl::PFNGLGETDEBUGMESSAGELOGPROC>(glfwGetProcAddress("glGetDebugMessageLogARB"));
                gl::GetPointerv = reinterpret_cast<gl::PFNGLGETPOINTERVPROC>(glfwGetProcAddress("glGetPointerv"));
                assert(gl::DebugMessageControl != nullptr);
                assert(gl::DebugMessageInsert != nullptr);
                assert(gl::DebugMessageCallback != nullptr);
                assert(gl::GetDebugMessageLog != nullptr);
                assert(gl::GetPointerv != nullptr);
            }

            if (extensions.find("GL_EXT_debug_marker") != std::string::npos) {
                gl::InsertEventMarkerEXT = reinterpret_cast<gl::PFNGLINSERTEVENTMARKEREXTPROC>(glfwGetProcAddress("glInsertEventMarkerEXT"));
                gl::PushGroupMarkerEXT = reinterpret_cast<gl::PFNGLPUSHGROUPMARKEREXTPROC>(glfwGetProcAddress("glPushGroupMarkerEXT"));
                gl::PopGroupMarkerEXT = reinterpret_cast<gl::PFNGLPOPGROUPMARKEREXTPROC>(glfwGetProcAddress("glPopGroupMarkerEXT"));
                assert(gl::InsertEventMarkerEXT != nullptr);
                assert(gl::PushGroupMarkerEXT != nullptr);
                assert(gl::PopGroupMarkerEXT != nullptr);
            }

            if (extensions.find("GL_EXT_debug_label") != std::string::npos) {
                gl::LabelObjectEXT = reinterpret_cast<gl::PFNGLLABELOBJECTEXTPROC>(glfwGetProcAddress("glLabelObjectEXT"));
                gl::GetObjectLabelEXT = reinterpret_cast<gl::PFNGLGETOBJECTLABELEXTPROC>(glfwGetProcAddress("glGetObjectLabelEXT"));
                assert(gl::LabelObjectEXT != nullptr);
                assert(gl::GetObjectLabelEXT != nullptr);
            }
        }

        if (extensions.find("GL_ARB_vertex_array_object") != std::string::npos) {
            gl::BindVertexArray = reinterpret_cast<gl::PFNGLBINDVERTEXARRAYPROC>(glfwGetProcAddress("glBindVertexArray"));
            gl::DeleteVertexArrays = reinterpret_cast<gl::PFNGLDELETEVERTEXARRAYSPROC>(glfwGetProcAddress("glDeleteVertexArrays"));
            gl::GenVertexArrays = reinterpret_cast<gl::PFNGLGENVERTEXARRAYSPROC>(glfwGetProcAddress("glGenVertexArrays"));
            gl::IsVertexArray = reinterpret_cast<gl::PFNGLISVERTEXARRAYPROC>(glfwGetProcAddress("glIsVertexArray"));
            assert(gl::BindVertexArray != nullptr);
            assert(gl::DeleteVertexArrays != nullptr);
            assert(gl::GenVertexArrays != nullptr);
            assert(gl::IsVertexArray != nullptr);
        } else if (extensions.find("GL_APPLE_vertex_array_object") != std::string::npos) {
            gl::BindVertexArray = reinterpret_cast<gl::PFNGLBINDVERTEXARRAYPROC>(glfwGetProcAddress("glBindVertexArrayAPPLE"));
            gl::DeleteVertexArrays = reinterpret_cast<gl::PFNGLDELETEVERTEXARRAYSPROC>(glfwGetProcAddress("glDeleteVertexArraysAPPLE"));
            gl::GenVertexArrays = reinterpret_cast<gl::PFNGLGENVERTEXARRAYSPROC>(glfwGetProcAddress("glGenVertexArraysAPPLE"));
            gl::IsVertexArray = reinterpret_cast<gl::PFNGLISVERTEXARRAYPROC>(glfwGetProcAddress("glIsVertexArrayAPPLE"));
            assert(gl::BindVertexArray != nullptr);
            assert(gl::DeleteVertexArrays != nullptr);
            assert(gl::GenVertexArrays != nullptr);
            assert(gl::IsVertexArray != nullptr);
        }

        // Require packed depth stencil
        gl::isPackedDepthStencilSupported = true;
        gl::isDepth24Supported = true;
    });

    glfwMakeContextCurrent(nullptr);
}

void GLFWView::onKey(GLFWwindow *window, int key, int /*scancode*/, int action, int mods) {
    GLFWView *view = reinterpret_cast<GLFWView *>(glfwGetWindowUserPointer(window));

    if (action == GLFW_RELEASE) {
        switch (key) {
        case GLFW_KEY_ESCAPE:
            glfwSetWindowShouldClose(window, true);
            break;
        case GLFW_KEY_TAB:
            view->map->toggleDebug();
            break;
        case GLFW_KEY_X:
            if (!mods)
                view->map->resetPosition();
            break;
        case GLFW_KEY_R:
            if (!mods) {
                view->map->setDefaultTransitionDuration(std::chrono::milliseconds(300));
                if (view->map->hasClass("night")) {
                    view->map->removeClass("night");
                } else {
                    view->map->addClass("night");
                }
            }
            break;
        case GLFW_KEY_N:
            if (!mods)
                view->map->resetNorth();
            break;
        }
    }
}

void GLFWView::onScroll(GLFWwindow *window, double /*xOffset*/, double yOffset) {
    GLFWView *view = reinterpret_cast<GLFWView *>(glfwGetWindowUserPointer(window));
    double delta = yOffset * 40;

    bool isWheel = delta != 0 && std::fmod(delta, 4.000244140625) == 0;

    double absDelta = delta < 0 ? -delta : delta;
    double scale = 2.0 / (1.0 + std::exp(-absDelta / 100.0));

    // Make the scroll wheel a bit slower.
    if (!isWheel) {
        scale = (scale - 1.0) / 2.0 + 1.0;
    }

    // Zooming out.
    if (delta < 0 && scale != 0) {
        scale = 1.0 / scale;
    }

    view->map->scaleBy(scale, view->lastX, view->lastY);
}

void GLFWView::onResize(GLFWwindow *window, int width, int height ) {
    GLFWView *view = reinterpret_cast<GLFWView *>(glfwGetWindowUserPointer(window));

    int fbWidth, fbHeight;
    glfwGetFramebufferSize(window, &fbWidth, &fbHeight);

    view->resize(width, height, static_cast<float>(fbWidth) / static_cast<float>(width), fbWidth, fbHeight);
}

void GLFWView::onMouseClick(GLFWwindow *window, int button, int action, int modifiers) {
    GLFWView *view = reinterpret_cast<GLFWView *>(glfwGetWindowUserPointer(window));

    if (button == GLFW_MOUSE_BUTTON_RIGHT ||
        (button == GLFW_MOUSE_BUTTON_LEFT && modifiers & GLFW_MOD_CONTROL)) {
        view->rotating = action == GLFW_PRESS;
        view->map->setGestureInProgress(view->rotating);
    } else if (button == GLFW_MOUSE_BUTTON_LEFT) {
        view->tracking = action == GLFW_PRESS;
        view->map->setGestureInProgress(view->tracking);

        if (action == GLFW_RELEASE) {
            double now = glfwGetTime();
            if (now - view->lastClick < 0.4 /* ms */) {
                if (modifiers & GLFW_MOD_SHIFT) {
                    view->map->scaleBy(0.5, view->lastX, view->lastY, std::chrono::milliseconds(500));
                } else {
                    view->map->scaleBy(2.0, view->lastX, view->lastY, std::chrono::milliseconds(500));
                }
            }
            view->lastClick = now;
        }
    }
}

void GLFWView::onMouseMove(GLFWwindow *window, double x, double y) {
    GLFWView *view = reinterpret_cast<GLFWView *>(glfwGetWindowUserPointer(window));
    if (view->tracking) {
        double dx = x - view->lastX;
        double dy = y - view->lastY;
        if (dx || dy) {
            view->map->moveBy(dx, dy);
        }
    } else if (view->rotating) {
        view->map->rotateBy(view->lastX, view->lastY, x, y);
    }
    view->lastX = x;
    view->lastY = y;
}

int GLFWView::run() {
    map->start();

    while (!glfwWindowShouldClose(window)) {
        glfwWaitEvents();
    }

    map->stop([]() {
        glfwWaitEvents();
    });

    return 0;
}

void GLFWView::activate() {
    glfwMakeContextCurrent(window);
}

void GLFWView::deactivate() {
    glfwMakeContextCurrent(nullptr);
}

void GLFWView::notify() {
    glfwPostEmptyEvent();
}

void GLFWView::invalidate(std::function<void()> render) {
    render();
    glfwSwapBuffers(window);
    fps();
}

void GLFWView::fps() {
    static int frames = 0;
    static double timeElapsed = 0;

    frames++;
    double currentTime = glfwGetTime();

    if (currentTime - timeElapsed >= 1) {
        mbgl::Log::Info(mbgl::Event::OpenGL, "FPS: %4.2f", frames / (currentTime - timeElapsed));
        timeElapsed = currentTime;
        frames = 0;
    }
}

namespace mbgl {
namespace platform {

double elapsed() { return glfwGetTime(); }

#ifndef GL_ES_VERSION_2_0
void showDebugImage(std::string name, const char *data, size_t width, size_t height) {
    glfwInit();

    static GLFWwindow *debugWindow = nullptr;
    if (!debugWindow) {
        debugWindow = glfwCreateWindow(width, height, name.c_str(), nullptr, nullptr);
        if (!debugWindow) {
            glfwTerminate();
            fprintf(stderr, "Failed to initialize window\n");
            exit(1);
        }
    }

    GLFWwindow *currentWindow = glfwGetCurrentContext();

    glfwSetWindowSize(debugWindow, width, height);
    glfwMakeContextCurrent(debugWindow);

    int fbWidth, fbHeight;
    glfwGetFramebufferSize(debugWindow, &fbWidth, &fbHeight);
    float scale = static_cast<float>(fbWidth) / static_cast<float>(width);

    MBGL_CHECK_ERROR(glPixelZoom(scale, -scale));
    MBGL_CHECK_ERROR(glRasterPos2f(-1.0f, 1.0f));
    MBGL_CHECK_ERROR(glDrawPixels(width, height, GL_LUMINANCE, GL_UNSIGNED_BYTE, data));
    glfwSwapBuffers(debugWindow);

    glfwMakeContextCurrent(currentWindow);
}


void showColorDebugImage(std::string name, const char *data, size_t logicalWidth, size_t logicalHeight, size_t width, size_t height) {
    glfwInit();

    static GLFWwindow *debugWindow = nullptr;
    if (!debugWindow) {
        debugWindow = glfwCreateWindow(logicalWidth, logicalHeight, name.c_str(), nullptr, nullptr);
        if (!debugWindow) {
            glfwTerminate();
            fprintf(stderr, "Failed to initialize window\n");
            exit(1);
        }
    }

    GLFWwindow *currentWindow = glfwGetCurrentContext();

    glfwSetWindowSize(debugWindow, logicalWidth, logicalHeight);
    glfwMakeContextCurrent(debugWindow);

    int fbWidth, fbHeight;
    glfwGetFramebufferSize(debugWindow, &fbWidth, &fbHeight);
    float xScale = static_cast<float>(fbWidth) / static_cast<float>(width);
    float yScale = static_cast<float>(fbHeight) / static_cast<float>(height);

    MBGL_CHECK_ERROR(glClearColor(0.8, 0.8, 0.8, 1));
    MBGL_CHECK_ERROR(glClear(GL_COLOR_BUFFER_BIT));
    MBGL_CHECK_ERROR(glEnable(GL_BLEND));
    MBGL_CHECK_ERROR(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

    MBGL_CHECK_ERROR(glPixelZoom(xScale, -yScale));
    MBGL_CHECK_ERROR(glRasterPos2f(-1.0f, 1.0f));
    MBGL_CHECK_ERROR(glDrawPixels(width, height, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, data));
    glfwSwapBuffers(debugWindow);

    glfwMakeContextCurrent(currentWindow);
}
#endif

}
}
