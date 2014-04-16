#include "glfw_view.hpp"

MapView::MapView(llmr::Settings &settings, bool fullscreen)
    : fullscreen(fullscreen), settings(settings), map(settings) {}

MapView::~MapView() { glfwTerminate(); }

void MapView::init() {
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize glfw\n");
        exit(1);
    }

    GLFWmonitor *monitor = nullptr;
    if (fullscreen) {
        monitor = glfwGetPrimaryMonitor();
    }

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

    window = glfwCreateWindow(1024, 768, "llmr", monitor, NULL);
    if (!window) {
        glfwTerminate();
        fprintf(stderr, "Failed to initialize window\n");
        exit(1);
    }

    glfwSetWindowUserPointer(window, this);
    glfwMakeContextCurrent(window);

    int width, height;
    glfwGetWindowSize(window, &width, &height);
    int fb_width, fb_height;
    glfwGetFramebufferSize(window, &fb_width, &fb_height);

    settings.load();
    map.setup();

    resize(window, 0, 0);

    glfwSwapInterval(1);

    map.loadSettings();

    glfwSetCursorPosCallback(window, mousemove);
    glfwSetMouseButtonCallback(window, mouseclick);
    glfwSetWindowSizeCallback(window, resize);
    glfwSetFramebufferSizeCallback(window, resize);
    glfwSetScrollCallback(window, scroll);
    glfwSetKeyCallback(window, key);
}

void MapView::key(GLFWwindow *window, int key, int /*scancode*/, int action, int mods) {
    MapView *mapView = (MapView *)glfwGetWindowUserPointer(window);

    if (action == GLFW_RELEASE) {
        switch (key) {
        case GLFW_KEY_ESCAPE:
            glfwSetWindowShouldClose(window, true);
            break;
        case GLFW_KEY_TAB:
            mapView->map.toggleDebug();
            break;
        case GLFW_KEY_X:
            if (!mods)
                mapView->map.resetPosition();
            break;
        case GLFW_KEY_R:
            if (!mods)
                mapView->map.toggleRaster();
            break;
        case GLFW_KEY_N:
            if (!mods)
                mapView->map.resetNorth();
            break;
        }
    }
}

void MapView::scroll(GLFWwindow *window, double /*xoffset*/, double yoffset) {
    MapView *mapView = (MapView *)glfwGetWindowUserPointer(window);
    double delta = yoffset * 40;

    bool is_wheel = delta != 0 && fmod(delta, 4.000244140625) == 0;

    double absdelta = delta < 0 ? -delta : delta;
    double scale = 2.0 / (1.0 + exp(-absdelta / 100.0));

    // Make the scroll wheel a bit slower.
    if (!is_wheel) {
        scale = (scale - 1.0) / 2.0 + 1.0;
    }

    // Zooming out.
    if (delta < 0 && scale != 0) {
        scale = 1.0 / scale;
    }

    mapView->map.startScaling();
    mapView->map.scaleBy(scale, mapView->last_x, mapView->last_y);
}

void MapView::resize(GLFWwindow *window, int, int) {
    MapView *mapView = (MapView *)glfwGetWindowUserPointer(window);

    int width, height;
    glfwGetWindowSize(window, &width, &height);
    int fb_width, fb_height;
    glfwGetFramebufferSize(window, &fb_width, &fb_height);

    mapView->map.resize(width, height, (float)fb_width / (float)width, fb_width, fb_height);
    mapView->map.update();
}

void MapView::mouseclick(GLFWwindow *window, int button, int action, int modifiers) {
    MapView *mapView = (MapView *)glfwGetWindowUserPointer(window);

    if (button == GLFW_MOUSE_BUTTON_RIGHT ||
        (button == GLFW_MOUSE_BUTTON_LEFT && modifiers & GLFW_MOD_CONTROL)) {
        mapView->rotating = action == GLFW_PRESS;
        if (!mapView->rotating) {
            mapView->map.stopRotating();
        }
    } else if (button == GLFW_MOUSE_BUTTON_LEFT) {
        mapView->tracking = action == GLFW_PRESS;

        if (action == GLFW_RELEASE) {
            mapView->map.stopPanning();
            double now = glfwGetTime();
            if (now - mapView->last_click < 0.4 /* ms */) {
                mapView->map.scaleBy(2.0, mapView->last_x, mapView->last_y, 0.5);
            }
            mapView->last_click = now;
        }
    }
}

void MapView::mousemove(GLFWwindow *window, double x, double y) {
    MapView *mapView = (MapView *)glfwGetWindowUserPointer(window);
    if (mapView->tracking) {
        double dx = x - mapView->last_x;
        double dy = y - mapView->last_y;
        if (dx || dy) {
            mapView->map.startPanning();
            mapView->map.moveBy(dx, dy);
        }
    } else if (mapView->rotating) {
        mapView->map.startRotating();
        mapView->map.rotateBy(mapView->last_x, mapView->last_y, x, y);
    }
    mapView->last_x = x;
    mapView->last_y = y;
}

int MapView::run() {
    while (!glfwWindowShouldClose(window)) {
        llmr::platform::cleanup();

        if (dirty) {
            try {
                dirty = map.render();
            }
            catch (std::exception &ex) {
                fprintf(stderr, "exception: %s\n", ex.what());
            }
            glfwSwapBuffers(window);
            fps();
        }

        if (dirty) {
            glfwPollEvents();
        } else {
            glfwWaitEvents();
        }
    }

    return 0;
}

void MapView::fps() {
    static int frames = 0;
    static double time_elapsed = 0;

    frames++;
    double current_time = glfwGetTime();

    if (current_time - time_elapsed >= 1) {
        fprintf(stderr, "FPS: %4.2f\n", frames / (current_time - time_elapsed));
        time_elapsed = current_time;
        frames = 0;
    }
}

namespace llmr {
namespace platform {

double elapsed() { return glfwGetTime(); }

void show_debug_image(std::string name, const char *data, size_t width, size_t height) {
    static GLFWwindow *debug_window = nullptr;
    if (!debug_window) {
        debug_window = glfwCreateWindow(width, height, name.c_str(), nullptr, nullptr);
        if (!debug_window) {
            glfwTerminate();
            fprintf(stderr, "Failed to initialize window\n");
            exit(1);
        }
    }

    GLFWwindow *current_window = glfwGetCurrentContext();

    glfwSetWindowSize(debug_window, width, height);
    glfwMakeContextCurrent(debug_window);

    int fb_width, fb_height;
    glfwGetFramebufferSize(debug_window, &fb_width, &fb_height);
    float scale = (float)fb_width / (float)width;

    glPixelZoom(scale, -scale);
    glRasterPos2f(-1.0f, 1.0f);
    glDrawPixels(width, height, GL_LUMINANCE, GL_UNSIGNED_BYTE, data);
    glfwSwapBuffers(debug_window);

    glfwMakeContextCurrent(current_window);
}
}
}
