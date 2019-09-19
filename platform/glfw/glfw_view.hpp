#pragma once

#include <mbgl/map/map.hpp>
#include <mbgl/util/geometry.hpp>
#include <mbgl/util/optional.hpp>
#include <mbgl/util/run_loop.hpp>
#include <mbgl/util/timer.hpp>

struct GLFWwindow;
class GLFWBackend;
class GLFWRendererFrontend;

namespace mbgl {
namespace gfx {
class RendererBackend;
} // namespace gfx
} // namespace mbgl

class GLFWView : public mbgl::MapObserver {
public:
    GLFWView(bool fullscreen, bool benchmark);
    ~GLFWView() override;

    float getPixelRatio() const;

    void setMap(mbgl::Map*);
    
    void setRenderFrontend(GLFWRendererFrontend*);

    mbgl::gfx::RendererBackend& getRendererBackend();

    // Callback called when the user presses the key mapped to style change.
    // The expected action is to set a new style, different to the current one.
    void setChangeStyleCallback(std::function<void()> callback);

    void setPauseResumeCallback(std::function<void()> callback) {
        pauseResumeCallback = callback;
    };

    void setOnlineStatusCallback(std::function<void()> callback) {
        onlineStatusCallback = callback;
    }

    void setResetCacheCallback(std::function<void()> callback) {
        resetDatabaseCallback = callback;
    };

    void setShouldClose();

    void setWindowTitle(const std::string&);

    void run();
    
    void invalidate();

    mbgl::Size getSize() const;

    // mbgl::MapObserver implementation
    void onDidFinishLoadingStyle() override;

protected:
    // mbgl::Backend implementation

private:
    // Window callbacks
    static void onKey(GLFWwindow *window, int key, int scancode, int action, int mods);
    static void onScroll(GLFWwindow *window, double xoffset, double yoffset);
    static void onWindowResize(GLFWwindow *window, int width, int height);
    static void onFramebufferResize(GLFWwindow *window, int width, int height);
    static void onMouseClick(GLFWwindow *window, int button, int action, int modifiers);
    static void onMouseMove(GLFWwindow *window, double x, double y);
    static void onWindowFocus(GLFWwindow *window, int focused);

    // Internal
    void report(float duration);

    mbgl::Color makeRandomColor() const;
    mbgl::Point<double> makeRandomPoint() const;
    static std::unique_ptr<mbgl::style::Image> makeImage(const std::string& id, int width, int height, float pixelRatio);

    void nextOrientation();

    void addRandomPointAnnotations(int count);
    void addRandomLineAnnotations(int count);
    void addRandomShapeAnnotations(int count);
    void addRandomCustomPointAnnotations(int count);
    void addAnimatedAnnotation();
    void updateAnimatedAnnotations();
    void toggleCustomSource();

    void clearAnnotations();
    void popAnnotation();

    mbgl::AnnotationIDs annotationIDs;
    std::vector<std::string> spriteIDs;

    mbgl::AnnotationIDs animatedAnnotationIDs;
    std::vector<double> animatedAnnotationAddedTimes;

private:
    void toggle3DExtrusions(bool visible);

    mbgl::Map* map = nullptr;
    GLFWRendererFrontend* rendererFrontend = nullptr;
    std::unique_ptr<GLFWBackend> backend;

    bool fullscreen = false;
    const bool benchmark = false;
    bool tracking = false;
    bool rotating = false;
    bool pitching = false;
    bool show3DExtrusions = false;

    // Frame timer
    int frames = 0;
    float frameTime = 0;
    double lastReported = 0;

    int width = 1024;
    int height = 768;
    float pixelRatio;

    double lastX = 0, lastY = 0;

    double lastClick = -1;

    std::function<void()> changeStyleCallback;
    std::function<void()> pauseResumeCallback;
    std::function<void()> onlineStatusCallback;
    std::function<void()> resetDatabaseCallback;
    std::function<void(mbgl::Map*)> animateRouteCallback;

    mbgl::util::RunLoop runLoop;
    mbgl::util::Timer frameTick;

    GLFWwindow *window = nullptr;
    bool dirty = false;
    mbgl::optional<std::string> featureID;
};
