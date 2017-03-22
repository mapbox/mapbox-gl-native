#pragma once

#include <mbgl/gl/gl.hpp>
#include <mbgl/map/map.hpp>
#include <mbgl/map/view.hpp>
#include <mbgl/map/backend.hpp>
#include <mbgl/util/run_loop.hpp>
#include <mbgl/util/timer.hpp>
#include <mbgl/util/geometry.hpp>

#if MBGL_USE_GLES2
#define GLFW_INCLUDE_ES2
#endif
#define GL_GLEXT_PROTOTYPES
#include <GLFW/glfw3.h>

class GLFWView : public mbgl::View, public mbgl::Backend {
public:
    GLFWView(bool fullscreen = false, bool benchmark = false);
    ~GLFWView() override;

    float getPixelRatio() const;

    void setMap(mbgl::Map*);

    // Callback called when the user presses the key mapped to style change.
    // The expected action is to set a new style, different to the current one.
    void setChangeStyleCallback(std::function<void()> callback);

    void setPauseResumeCallback(std::function<void()> callback) {
        pauseResumeCallback = callback;
    };

    void setShouldClose();

    void setWindowTitle(const std::string&);

    void run();

    // mbgl::View implementation
    void bind() override;
    mbgl::Size getSize() const;
    mbgl::Size getFramebufferSize() const;

    // mbgl::Backend implementation
    void invalidate() override;
    void updateAssumedState() override;

protected:
    // mbgl::Backend implementation
    mbgl::gl::ProcAddress initializeExtension(const char*) override;
    void activate() override;
    void deactivate() override;

private:
    // Window callbacks
    static void onKey(GLFWwindow *window, int key, int scancode, int action, int mods);
    static void onScroll(GLFWwindow *window, double xoffset, double yoffset);
    static void onWindowResize(GLFWwindow *window, int width, int height);
    static void onFramebufferResize(GLFWwindow *window, int width, int height);
    static void onMouseClick(GLFWwindow *window, int button, int action, int modifiers);
    static void onMouseMove(GLFWwindow *window, double x, double y);

    // Internal
    void report(float duration);

    mbgl::Color makeRandomColor() const;
    mbgl::Point<double> makeRandomPoint() const;
    static std::shared_ptr<const mbgl::SpriteImage>
    makeSpriteImage(int width, int height, float pixelRatio);

    void nextOrientation();

    void addRandomPointAnnotations(int count);
    void addRandomLineAnnotations(int count);
    void addRandomShapeAnnotations(int count);
    void addRandomCustomPointAnnotations(int count);

    void clearAnnotations();
    void popAnnotation();

    mbgl::AnnotationIDs annotationIDs;
    std::vector<std::string> spriteIDs;

private:
    mbgl::Map* map = nullptr;

    bool fullscreen = false;
    const bool benchmark = false;
    bool tracking = false;
    bool rotating = false;
    bool pitching = false;

    // Frame timer
    int frames = 0;
    float frameTime = 0;
    double lastReported = 0;

    int width = 1024;
    int height = 768;
    int fbWidth;
    int fbHeight;
    float pixelRatio;

    double lastX = 0, lastY = 0;

    double lastClick = -1;

    std::function<void()> changeStyleCallback;
    std::function<void()> pauseResumeCallback;

    mbgl::util::RunLoop runLoop;
    mbgl::util::Timer frameTick;

    GLFWwindow *window = nullptr;
    bool dirty = false;
};
