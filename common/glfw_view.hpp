#ifndef LLMR_COMMON_GLFW_VIEW
#define LLMR_COMMON_GLFW_VIEW

#include <GLFW/glfw3.h>
#include <llmr/llmr.hpp>

class MapView {
public:
    MapView(llmr::Settings &settings, bool fullscreen = false);
    ~MapView();

    void init();

    static void key(GLFWwindow *window, int key, int scancode, int action, int mods);
    static void scroll(GLFWwindow *window, double xoffset, double yoffset);
    static void resize(GLFWwindow *window, int, int);
    static void mouseclick(GLFWwindow *window, int button, int action, int modifiers);
    static void mousemove(GLFWwindow *window, double x, double y);

    int run();
    void fps();

public:
    bool fullscreen = false;

    bool dirty = true;
    double last_x = 0, last_y = 0;
    bool tracking = false;

    bool rotating = false;

    double last_click = -1;

    GLFWwindow *window = nullptr;
    llmr::Settings &settings;
    llmr::Map map;
};

#endif
