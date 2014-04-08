#ifndef LLMR_COMMON_GLFW_VIEW
#define LLMR_COMMON_GLFW_VIEW

#include <GLFW/glfw3.h>
#include <llmr/llmr.hpp>
#include <llmr/platform/view.hpp>

namespace llmr {

class GLFWView : ViewContext {
public:
    GLFWView(Map &map, bool fullscreen = false);
    ~GLFWView();

    // ViewContext implementation
    void swap();
    void make_active();

    // public API
    void show();
    void close();
    void fps();

private:
    // GLFW window callbacks
    static void character(GLFWwindow *window, unsigned int codepoint);
    static void key(GLFWwindow *window, int key, int scancode, int action, int mods);
    static void scroll(GLFWwindow *window, double xoffset, double yoffset);
    static void resize(GLFWwindow *window, int = 0, int = 0);
    static void mouseclick(GLFWwindow *window, int button, int action, int modifiers);
    static void mousemove(GLFWwindow *window, double x, double y);

private:
    bool fullscreen = false;

    bool dirty = true;
    double last_x = 0, last_y = 0;
    bool tracking = false;

    double start_x = 0, start_y = 0;
    bool rotating = false;

    double last_click = -1;

    GLFWwindow *window = nullptr;
    llmr::Map& map;
};

}

#endif
