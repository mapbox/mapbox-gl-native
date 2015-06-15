#include <mbgl/platform/default/glfw_view.hpp>
#include <mbgl/platform/gl.hpp>
#include <mbgl/platform/log.hpp>
#include <mbgl/util/gl_helper.hpp>

#include <cassert>
#include <cstdlib>

void glfwError(int error, const char *description) {
    mbgl::Log::Error(mbgl::Event::OpenGL, "GLFW error (%i): %s", error, description);
    assert(false);
}

GLFWView::GLFWView(bool fullscreen_) : fullscreen(fullscreen_) {
    glfwSetErrorCallback(glfwError);

    std::srand(std::time(0));

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

    window = glfwCreateWindow(width, height, "Mapbox GL", monitor, NULL);
    if (!window) {
        glfwTerminate();
        mbgl::Log::Error(mbgl::Event::OpenGL, "failed to initialize window");
        exit(1);
    }

    glfwSetWindowUserPointer(window, this);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    glfwSetCursorPosCallback(window, onMouseMove);
    glfwSetMouseButtonCallback(window, onMouseClick);
    glfwSetWindowSizeCallback(window, onResize);
    glfwSetFramebufferSizeCallback(window, onResize);
    glfwSetScrollCallback(window, onScroll);
    glfwSetKeyCallback(window, onKey);

    mbgl::gl::InitializeExtensions(glfwGetProcAddress);

    glfwMakeContextCurrent(nullptr);
}

GLFWView::~GLFWView() {
    glfwDestroyWindow(window);
    glfwTerminate();
}

void GLFWView::initialize(mbgl::Map *map_) {
    View::initialize(map_);

    glfwGetWindowSize(window, &width, &height);
    onResize(window, width, height);
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
        case GLFW_KEY_C:
            view->map->toggleCollisionDebug();
            break;
        case GLFW_KEY_X:
            if (!mods)
                view->map->resetPosition();
            break;
        case GLFW_KEY_S:
            if (view->changeStyleCallback)
                view->changeStyleCallback();
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

    if (action == GLFW_RELEASE || action == GLFW_REPEAT) {
        switch (key) {
        case GLFW_KEY_1: view->addRandomPointAnnotations(1); break;
        case GLFW_KEY_2: view->addRandomPointAnnotations(10); break;
        case GLFW_KEY_3: view->addRandomPointAnnotations(100); break;
        case GLFW_KEY_4: view->addRandomPointAnnotations(1000); break;
        case GLFW_KEY_5: view->addRandomPointAnnotations(10000); break;
        case GLFW_KEY_6: view->addRandomPointAnnotations(100000); break;
        }
    }
}

void GLFWView::addRandomPointAnnotations(int count) {
    std::vector<mbgl::LatLng> points;
    std::vector<std::string> markers;

    const auto sw = map->latLngForPixel({ 0, 0 });
    const auto ne = map->latLngForPixel({ double(width), double(height) });

    for (int i = 0; i < count; i++) {
        const double lon = sw.longitude + (ne.longitude - sw.longitude) * (double(std::rand()) / RAND_MAX);
        const double lat = sw.latitude + (ne.latitude - sw.latitude) * (double(std::rand()) / RAND_MAX);

        points.push_back({ lat, lon });
        markers.push_back("default_marker");
    }

    map->addPointAnnotations(points, markers);
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
    view->width = width;
    view->height = height;

    int fbWidth, fbHeight;
    glfwGetFramebufferSize(window, &fbWidth, &fbHeight);

    view->map->resize(width, height, static_cast<float>(fbWidth) / static_cast<float>(width));
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

void GLFWView::run() {
    while (!glfwWindowShouldClose(window)) {
        glfwWaitEvents();
    }
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

void GLFWView::setChangeStyleCallback(std::function<void()> callback) {
    changeStyleCallback = callback;
}

void GLFWView::setShouldClose() {
    glfwSetWindowShouldClose(window, true);
    glfwPostEmptyEvent();
}

void GLFWView::setWindowTitle(const std::string& title) {
    glfwSetWindowTitle(window, (std::string { "Mapbox GL: " } + title).c_str());
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

    {
        gl::PreservePixelZoom pixelZoom;
        gl::PreserveRasterPos rasterPos;

        MBGL_CHECK_ERROR(glPixelZoom(scale, -scale));
        MBGL_CHECK_ERROR(glRasterPos2f(-1.0f, 1.0f));
        MBGL_CHECK_ERROR(glDrawPixels(width, height, GL_LUMINANCE, GL_UNSIGNED_BYTE, data));
    }

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

    {
        gl::PreserveClearColor clearColor;
        gl::PreserveBlend blend;
        gl::PreserveBlendFunc blendFunc;
        gl::PreservePixelZoom pixelZoom;
        gl::PreserveRasterPos rasterPos;

        MBGL_CHECK_ERROR(glClearColor(0.8, 0.8, 0.8, 1));
        MBGL_CHECK_ERROR(glClear(GL_COLOR_BUFFER_BIT));
        MBGL_CHECK_ERROR(glEnable(GL_BLEND));
        MBGL_CHECK_ERROR(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
        MBGL_CHECK_ERROR(glPixelZoom(xScale, -yScale));
        MBGL_CHECK_ERROR(glRasterPos2f(-1.0f, 1.0f));
        MBGL_CHECK_ERROR(glDrawPixels(width, height, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, data));
    }

    glfwSwapBuffers(debugWindow);

    glfwMakeContextCurrent(currentWindow);
}
#endif

}
}
