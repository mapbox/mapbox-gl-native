#ifndef MBGL_COMMON_GLFW_VIEW
#define MBGL_COMMON_GLFW_VIEW

#include <mbgl/mbgl.hpp>
#include <mbgl/util/time.hpp>

#ifdef NVIDIA
#define GLFW_INCLUDE_ES2
#endif
#include <GLFW/glfw3.h>

class GLFWView : public mbgl::View {
public:
    GLFWView(bool fullscreen = false);
    ~GLFWView();

    void initialize(mbgl::Map *map);
    void swap();
    void make_active();
    void notify_map_change(mbgl::MapChange change, mbgl::timestamp delay = 0);

    static void key(GLFWwindow *window, int key, int scancode, int action, int mods);
    static void scroll(GLFWwindow *window, double xoffset, double yoffset);
    static void resize(GLFWwindow *window, int, int);
    static void mouseclick(GLFWwindow *window, int button, int action, int modifiers);
    static void mousemove(GLFWwindow *window, double x, double y);

    static void eventloop(void *arg);

    int run();
    void fps();

public:
    bool fullscreen = false;

    double last_x = 0, last_y = 0;
    bool tracking = false;

    bool rotating = false;

    double last_click = -1;

    GLFWwindow *window = nullptr;
};

#endif
