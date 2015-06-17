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

    void initialize(mbgl::Map *map) override;
    void activate() override;
    void deactivate() override;
    void notify() override;
    void invalidate(std::function<void()> render) override;

    static void onKey(GLFWwindow *window, int key, int scancode, int action, int mods);
    static void onScroll(GLFWwindow *window, double xoffset, double yoffset);
    static void onResize(GLFWwindow *window, int width, int height);
    static void onMouseClick(GLFWwindow *window, int button, int action, int modifiers);
    static void onMouseMove(GLFWwindow *window, double x, double y);

    // Callback called when the user presses the key mapped to style change.
    // The expected action is to set a new style, different to the current one.
    void setChangeStyleCallback(std::function<void()> callback);

    void setShouldClose();

    void setWindowTitle(const std::string&);

    void run();
    void fps();

private:
    mbgl::LatLng makeRandomPoint() const;

    void addRandomPointAnnotations(int count);
    void addRandomShapeAnnotations(int count);

    void clearAnnotations();
    void popAnnotation();

    mbgl::AnnotationIDs annotationIDs;

private:
    bool fullscreen = false;
    bool tracking = false;
    bool rotating = false;

    int width = 1024;
    int height = 768;

    double lastX = 0, lastY = 0;

    double lastClick = -1;

    std::function<void()> changeStyleCallback;

    GLFWwindow *window = nullptr;
};

#endif
