#include "glfw_view.hpp"

using namespace llmr;

GLFWView::GLFWView(Map &map, bool fullscreen) :
    fullscreen(fullscreen),
    map(map) {

    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize glfw\n");
        exit(1);
    }

    // Create window
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

    // Setup GLFW callbacks
    glfwSetCursorPosCallback(window, mousemove);
    glfwSetMouseButtonCallback(window, mouseclick);
    glfwSetWindowSizeCallback(window, resize);
    glfwSetFramebufferSizeCallback(window, resize);
    glfwSetScrollCallback(window, scroll);
    glfwSetCharCallback(window, character);
    glfwSetKeyCallback(window, key);
}

GLFWView::~GLFWView() {
    glfwTerminate();
}

void GLFWView::swap() {
    glfwPostEmptyEvent();
}

void GLFWView::make_active() {
    glfwMakeContextCurrent(window);
}

void GLFWView::show() {
    resize(window);

    map.start(this);

    while (!glfwWindowShouldClose(window)) {
        if (map.can_swap()) {
            glfwSwapBuffers(window);
            map.swapped();
            fps();
        }

        glfwWaitEvents();
    }

    map.stop();
}

void GLFWView::character(GLFWwindow *window, unsigned int codepoint) {

}

void GLFWView::key(GLFWwindow *window, int key, int scancode, int action, int mods) {
    GLFWView *view = static_cast<GLFWView *>(glfwGetWindowUserPointer(window));

    if (action == GLFW_RELEASE) {
        switch (key) {
            case GLFW_KEY_ESCAPE:
                glfwSetWindowShouldClose(window, true);
                break;
            case GLFW_KEY_TAB:
                // view->map.toggleDebug();
                break;
            case GLFW_KEY_X:
                if (!mods) view->map.resetPosition();
                break;
            case GLFW_KEY_R:
                // if (!mods) view->map.toggleRaster();
                break;
            case GLFW_KEY_N:
                if (!mods) view->map.resetNorth();
                break;
        }
    }
}

void GLFWView::scroll(GLFWwindow *window, double xoffset, double yoffset) {
    GLFWView *view = static_cast<GLFWView *>(glfwGetWindowUserPointer(window));
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

    // view->map.startScaling();
    view->map.scaleBy(scale, view->last_x, view->last_y);
}

void GLFWView::resize(GLFWwindow *window, int, int) {
    GLFWView *view = static_cast<GLFWView *>(glfwGetWindowUserPointer(window));

    int width, height;
    glfwGetWindowSize(window, &width, &height);
    int fb_width, fb_height;
    glfwGetFramebufferSize(window, &fb_width, &fb_height);

    view->map.resize(width, height, fb_width, fb_height);
}

void GLFWView::mouseclick(GLFWwindow *window, int button, int action, int modifiers) {
    GLFWView *view = static_cast<GLFWView *>(glfwGetWindowUserPointer(window));

    if (button == GLFW_MOUSE_BUTTON_RIGHT || (button == GLFW_MOUSE_BUTTON_LEFT && modifiers & GLFW_MOD_CONTROL)) {
        view->rotating = action == GLFW_PRESS;
        if (view->rotating) {
            view->start_x = view->last_x;
            view->start_y = view->last_y;
        } else {
            // view->map.stopRotating();
        }
    } else if (button == GLFW_MOUSE_BUTTON_LEFT) {
        view->tracking = action == GLFW_PRESS;

        if (action == GLFW_RELEASE) {
            // view->map.stopPanning();
            double now = glfwGetTime();
            if (now - view->last_click < 0.4) {
                view->map.scaleBy(2.0, view->last_x, view->last_y);
            }
            view->last_click = now;
        }
    }
}

void GLFWView::mousemove(GLFWwindow *window, double x, double y) {
    GLFWView *view = static_cast<GLFWView *>(glfwGetWindowUserPointer(window));
    if (view->tracking) {
        double dx = x - view->last_x;
        double dy = y - view->last_y;
        if (dx || dy) {
            // view->map.startPanning();
            view->map.moveBy(dx, dy);
        }
    } else if (view->rotating) {
        // view->map.startRotating();
        view->map.rotateBy(view->start_x, view->start_y, view->last_x, view->last_y, x, y);
    }
    view->last_x = x;
    view->last_y = y;
}

void GLFWView::fps() {
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
