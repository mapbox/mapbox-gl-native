#ifndef MBGL_COMMON_GLFW_VIEW
#define MBGL_COMMON_GLFW_VIEW

#include <mbgl/mbgl.hpp>

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
    void notifyMapChange(mbgl::MapChange change, std::chrono::steady_clock::duration delay = std::chrono::steady_clock::duration::zero());

    static void onKey(GLFWwindow *window, int key, int scancode, int action, int mods);
    static void onScroll(GLFWwindow *window, double xoffset, double yoffset);
    static void onResize(GLFWwindow *window, int width, int height);
    static void onMouseClick(GLFWwindow *window, int button, int action, int modifiers);
    static void onMouseMove(GLFWwindow *window, double x, double y);

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
