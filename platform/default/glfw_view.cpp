#include <mbgl/platform/default/glfw_view.hpp>
#include <mbgl/platform/gl.hpp>

GLFWView::GLFWView(bool fullscreen_) : fullscreen(fullscreen_) {
#ifdef NVIDIA
    glDiscardFramebufferEXT = (PFNGLDISCARDFRAMEBUFFEREXTPROC)glfwGetProcAddress("glDiscardFramebufferEXT");
#endif
}

GLFWView::~GLFWView() {
    glfwTerminate();
}

void GLFWView::initialize(mbgl::Map *map_) {
    View::initialize(map_);

    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize glfw\n");
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
        fprintf(stderr, "Failed to initialize window\n");
        exit(1);
    }

    glfwSetWindowUserPointer(window, this);
    glfwMakeContextCurrent(window);

    int width, height;
    glfwGetWindowSize(window, &width, &height);
    int fb_width, fb_height;
    glfwGetFramebufferSize(window, &fb_width, &fb_height);

    resize(window, 0, 0);

    glfwSetCursorPosCallback(window, mousemove);
    glfwSetMouseButtonCallback(window, mouseclick);
    glfwSetWindowSizeCallback(window, resize);
    glfwSetFramebufferSizeCallback(window, resize);
    glfwSetScrollCallback(window, scroll);
    glfwSetKeyCallback(window, key);


    const std::string extensions = (char *)glGetString(GL_EXTENSIONS);
    {
        using namespace mbgl;

        if (extensions.find("GL_KHR_debug") != std::string::npos) {
            gl::DebugMessageControl = (gl::PFNGLDEBUGMESSAGECONTROLPROC)glfwGetProcAddress("glDebugMessageControl");
            gl::DebugMessageInsert = (gl::PFNGLDEBUGMESSAGEINSERTPROC)glfwGetProcAddress("glDebugMessageInsert");
            gl::DebugMessageCallback = (gl::PFNGLDEBUGMESSAGECALLBACKPROC)glfwGetProcAddress("glDebugMessageCallback");
            gl::GetDebugMessageLog = (gl::PFNGLGETDEBUGMESSAGELOGPROC)glfwGetProcAddress("glGetDebugMessageLog");
            gl::GetPointerv = (gl::PFNGLGETPOINTERVPROC)glfwGetProcAddress("glGetPointerv");
            gl::PushDebugGroup = (gl::PFNGLPUSHDEBUGGROUPPROC)glfwGetProcAddress("glPushDebugGroup");
            gl::PopDebugGroup = (gl::PFNGLPOPDEBUGGROUPPROC)glfwGetProcAddress("glPopDebugGroup");
            gl::ObjectLabel = (gl::PFNGLOBJECTLABELPROC)glfwGetProcAddress("glObjectLabel");
            gl::GetObjectLabel = (gl::PFNGLGETOBJECTLABELPROC)glfwGetProcAddress("glGetObjectLabel");
            gl::ObjectPtrLabel = (gl::PFNGLOBJECTPTRLABELPROC)glfwGetProcAddress("glObjectPtrLabel");
            gl::GetObjectPtrLabel = (gl::PFNGLGETOBJECTPTRLABELPROC)glfwGetProcAddress("glGetObjectPtrLabel");
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
                gl::DebugMessageControl = (gl::PFNGLDEBUGMESSAGECONTROLPROC)glfwGetProcAddress("glDebugMessageControlARB");
                gl::DebugMessageInsert = (gl::PFNGLDEBUGMESSAGEINSERTPROC)glfwGetProcAddress("glDebugMessageInsertARB");
                gl::DebugMessageCallback = (gl::PFNGLDEBUGMESSAGECALLBACKPROC)glfwGetProcAddress("glDebugMessageCallbackARB");
                gl::GetDebugMessageLog = (gl::PFNGLGETDEBUGMESSAGELOGPROC)glfwGetProcAddress("glGetDebugMessageLogARB");
                gl::GetPointerv = (gl::PFNGLGETPOINTERVPROC)glfwGetProcAddress("glGetPointerv");
                assert(gl::DebugMessageControl != nullptr);
                assert(gl::DebugMessageInsert != nullptr);
                assert(gl::DebugMessageCallback != nullptr);
                assert(gl::GetDebugMessageLog != nullptr);
                assert(gl::GetPointerv != nullptr);
            }

            if (extensions.find("GL_EXT_debug_marker") != std::string::npos) {
                gl::InsertEventMarkerEXT = (gl::PFNGLINSERTEVENTMARKEREXTPROC)glfwGetProcAddress("glInsertEventMarkerEXT");
                gl::PushGroupMarkerEXT = (gl::PFNGLPUSHGROUPMARKEREXTPROC)glfwGetProcAddress("glPushGroupMarkerEXT");
                gl::PopGroupMarkerEXT = (gl::PFNGLPOPGROUPMARKEREXTPROC)glfwGetProcAddress("glPopGroupMarkerEXT");
                assert(gl::InsertEventMarkerEXT != nullptr);
                assert(gl::PushGroupMarkerEXT != nullptr);
                assert(gl::PopGroupMarkerEXT != nullptr);
            }

            if (extensions.find("GL_EXT_debug_label") != std::string::npos) {
                gl::LabelObjectEXT = (gl::PFNGLLABELOBJECTEXTPROC)glfwGetProcAddress("glLabelObjectEXT");
                gl::GetObjectLabelEXT = (gl::PFNGLGETOBJECTLABELEXTPROC)glfwGetProcAddress("glGetObjectLabelEXT");
                assert(gl::LabelObjectEXT != nullptr);
                assert(gl::GetObjectLabelEXT != nullptr);
            }
        }

        if (extensions.find("GL_ARB_vertex_array_object") != std::string::npos) {
            gl::BindVertexArray = (gl::PFNGLBINDVERTEXARRAYPROC)glfwGetProcAddress("glBindVertexArray");
            gl::DeleteVertexArrays = (gl::PFNGLDELETEVERTEXARRAYSPROC)glfwGetProcAddress("glDeleteVertexArrays");
            gl::GenVertexArrays = (gl::PFNGLGENVERTEXARRAYSPROC)glfwGetProcAddress("glGenVertexArrays");
            gl::IsVertexArray = (gl::PFNGLISVERTEXARRAYPROC)glfwGetProcAddress("glIsVertexArray");
            assert(gl::BindVertexArray != nullptr);
            assert(gl::DeleteVertexArrays != nullptr);
            assert(gl::GenVertexArrays != nullptr);
            assert(gl::IsVertexArray != nullptr);
        } else if (extensions.find("GL_APPLE_vertex_array_object") != std::string::npos) {
            gl::BindVertexArray = (gl::PFNGLBINDVERTEXARRAYPROC)glfwGetProcAddress("glBindVertexArrayAPPLE");
            gl::DeleteVertexArrays = (gl::PFNGLDELETEVERTEXARRAYSPROC)glfwGetProcAddress("glDeleteVertexArraysAPPLE");
            gl::GenVertexArrays = (gl::PFNGLGENVERTEXARRAYSPROC)glfwGetProcAddress("glGenVertexArraysAPPLE");
            gl::IsVertexArray = (gl::PFNGLISVERTEXARRAYPROC)glfwGetProcAddress("glIsVertexArrayAPPLE");
            assert(gl::BindVertexArray != nullptr);
            assert(gl::DeleteVertexArrays != nullptr);
            assert(gl::GenVertexArrays != nullptr);
            assert(gl::IsVertexArray != nullptr);
        }

        if (extensions.find("GL_ARB_get_program_binary") != std::string::npos) {
            gl::GetProgramBinary = (gl::PFNGLGETPROGRAMBINARYPROC)glfwGetProcAddress("glGetProgramBinary");
            gl::ProgramBinary = (gl::PFNGLPROGRAMBINARYPROC)glfwGetProcAddress("glProgramBinary");
            gl::ProgramParameteri = (gl::PFNGLPROGRAMPARAMETERIPROC)glfwGetProcAddress("glProgramParameteri");
            assert(gl::GetProgramBinary != nullptr);
            assert(gl::ProgramBinary != nullptr);
            assert(gl::ProgramParameteri != nullptr);
        }
    }

    glfwMakeContextCurrent(nullptr);
}

void GLFWView::key(GLFWwindow *window, int key, int /*scancode*/, int action, int mods) {
    GLFWView *view = (GLFWView *)glfwGetWindowUserPointer(window);

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
                view->map->setDefaultTransitionDuration(300);
                view->map->toggleClass("night");
            }
            break;
        case GLFW_KEY_N:
            if (!mods)
                view->map->resetNorth();
            break;
        }
    }
}

void GLFWView::scroll(GLFWwindow *window, double /*xoffset*/, double yoffset) {
    GLFWView *view = (GLFWView *)glfwGetWindowUserPointer(window);
    double delta = yoffset * 40;

    bool is_wheel = delta != 0 && std::fmod(delta, 4.000244140625) == 0;

    double absdelta = delta < 0 ? -delta : delta;
    double scale = 2.0 / (1.0 + std::exp(-absdelta / 100.0));

    // Make the scroll wheel a bit slower.
    if (!is_wheel) {
        scale = (scale - 1.0) / 2.0 + 1.0;
    }

    // Zooming out.
    if (delta < 0 && scale != 0) {
        scale = 1.0 / scale;
    }

    view->map->startScaling();
    view->map->scaleBy(scale, view->last_x, view->last_y);
}

void GLFWView::resize(GLFWwindow *window, int, int) {
    GLFWView *view = (GLFWView *)glfwGetWindowUserPointer(window);

    int width, height;
    glfwGetWindowSize(window, &width, &height);
    int fb_width, fb_height;
    glfwGetFramebufferSize(window, &fb_width, &fb_height);

    view->map->resize(width, height, (float)fb_width / (float)width, fb_width, fb_height);
}

void GLFWView::mouseclick(GLFWwindow *window, int button, int action, int modifiers) {
    GLFWView *view = (GLFWView *)glfwGetWindowUserPointer(window);

    if (button == GLFW_MOUSE_BUTTON_RIGHT ||
        (button == GLFW_MOUSE_BUTTON_LEFT && modifiers & GLFW_MOD_CONTROL)) {
        view->rotating = action == GLFW_PRESS;
        if (!view->rotating) {
            view->map->stopRotating();
        }
    } else if (button == GLFW_MOUSE_BUTTON_LEFT) {
        view->tracking = action == GLFW_PRESS;

        if (action == GLFW_RELEASE) {
            view->map->stopPanning();
            double now = glfwGetTime();
            if (now - view->last_click < 0.4 /* ms */) {
                if (modifiers & GLFW_MOD_SHIFT) {
                    view->map->scaleBy(0.5, view->last_x, view->last_y, 0.5);
                } else {
                    view->map->scaleBy(2.0, view->last_x, view->last_y, 0.5);
                }
            }
            view->last_click = now;
        }
    }
}

void GLFWView::mousemove(GLFWwindow *window, double x, double y) {
    GLFWView *view = (GLFWView *)glfwGetWindowUserPointer(window);
    if (view->tracking) {
        double dx = x - view->last_x;
        double dy = y - view->last_y;
        if (dx || dy) {
            view->map->startPanning();
            view->map->moveBy(dx, dy);
        }
    } else if (view->rotating) {
        view->map->startRotating();
        view->map->rotateBy(view->last_x, view->last_y, x, y);
    }
    view->last_x = x;
    view->last_y = y;
}

int GLFWView::run() {
    map->start();

    while (!glfwWindowShouldClose(window)) {
        if (map->needsSwap()) {
            glfwSwapBuffers(window);
            map->swapped();
            fps();
        }

        glfwWaitEvents();
    }

    map->stop([]() {
        glfwWaitEvents();
    });

    return 0;
}

void GLFWView::make_active() {
    glfwMakeContextCurrent(window);
}

void GLFWView::make_inactive() {
    glfwMakeContextCurrent(nullptr);
}

void GLFWView::notify() {
    glfwPostEmptyEvent();
}

void GLFWView::swap() {
    glfwPostEmptyEvent();
}

void GLFWView::notify_map_change(mbgl::MapChange /*change*/, mbgl::timestamp /*delay*/) {
    // no-op
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

namespace mbgl {
namespace platform {

double elapsed() { return glfwGetTime(); }

#ifndef GL_ES_VERSION_2_0
void show_debug_image(std::string name, const char *data, size_t width, size_t height) {
    glfwInit();

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


void show_color_debug_image(std::string name, const char *data, size_t logical_width, size_t logical_height, size_t width, size_t height) {
    glfwInit();

    static GLFWwindow *debug_window = nullptr;
    if (!debug_window) {
        debug_window = glfwCreateWindow(logical_width, logical_height, name.c_str(), nullptr, nullptr);
        if (!debug_window) {
            glfwTerminate();
            fprintf(stderr, "Failed to initialize window\n");
            exit(1);
        }
    }

    GLFWwindow *current_window = glfwGetCurrentContext();

    glfwSetWindowSize(debug_window, logical_width, logical_height);
    glfwMakeContextCurrent(debug_window);

    int fb_width, fb_height;
    glfwGetFramebufferSize(debug_window, &fb_width, &fb_height);
    float x_scale = (float)fb_width / (float)width;
    float y_scale = (float)fb_height / (float)height;

    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glPixelZoom(x_scale, -y_scale);
    glRasterPos2f(-1.0f, 1.0f);
    glDrawPixels(width, height, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, data);
    glfwSwapBuffers(debug_window);

    glfwMakeContextCurrent(current_window);
}
#endif

}
}
