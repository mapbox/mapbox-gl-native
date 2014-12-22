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
    void activate();
    void deactivate();
    void notify();
    void notifyMapChange(mbgl::MapChange change, mbgl::timestamp delay = 0);

    static void key(GLFWwindow *window, int key, int scancode, int action, int mods);
    static void scroll(GLFWwindow *window, double xoffset, double yoffset);
    static void resize(GLFWwindow *window, int width, int height);
    static void mouseClick(GLFWwindow *window, int button, int action, int modifiers);
    static void mouseMove(GLFWwindow *window, double x, double y);

    static void eventloop(void *arg);

    int run();
    void fps();

public:
    bool fullscreen = false;

    double lastX = 0, lastY = 0;
    bool tracking = false;

    bool rotating = false;

    double lastClick = -1;

    GLFWwindow *window = nullptr;
};

#endif
