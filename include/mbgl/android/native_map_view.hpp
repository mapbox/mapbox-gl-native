#ifndef MBGL_ANDROID_NATIVE_MAP_VIEW
#define MBGL_ANDROID_NATIVE_MAP_VIEW

#include <mbgl/map/map.hpp>
#include <mbgl/map/view.hpp>
#include <mbgl/util/noncopyable.hpp>
#include <mbgl/storage/sqlite_cache.hpp>
#include <mbgl/storage/default_file_source.hpp>

#include <string>
#include <jni.h>
#include <android/native_window.h>
#include <EGL/egl.h>

namespace mbgl {
namespace android {

class NativeMapView : public mbgl::View, private mbgl::util::noncopyable {
public:
    NativeMapView(JNIEnv *env, jobject obj, float pixelRatio, int availableProcessors, size_t totalMemory);
    virtual ~NativeMapView();

    float getPixelRatio() const override;
    std::array<uint16_t, 2> getSize() const override;
    std::array<uint16_t, 2> getFramebufferSize() const override;
    void activate() override;
    void deactivate() override;
    void notify() override;
    void invalidate() override;
    void beforeRender() override;
    void afterRender() override;

    void notifyMapChange(mbgl::MapChange) override;

    mbgl::Map &getMap();
    mbgl::DefaultFileSource &getFileSource();

    void initializeDisplay();
    void terminateDisplay();

    void initializeContext();
    void terminateContext();

    void createSurface(ANativeWindow *window);
    void destroySurface();

    void resume();
    void pause();

    void enableFps(bool enable);
    void updateFps();

    void renderSync();

    void resizeView(int width, int height);
    void resizeFramebuffer(int width, int height);

private:
    EGLConfig chooseConfig(const EGLConfig configs[], EGLint numConfigs);

    bool inEmulator();

private:
    JavaVM *vm = nullptr;
    jweak obj = nullptr;

    ANativeWindow *window = nullptr;
    EGLDisplay display = EGL_NO_DISPLAY;
    EGLSurface surface = EGL_NO_SURFACE;
    EGLContext context = EGL_NO_CONTEXT;

    EGLConfig config = nullptr;
    EGLint format = -1;

    std::string styleUrl;
    std::string apiKey;

    bool firstTime = false;

    bool fpsEnabled = false;
    double fps = 0.0;

    int width = 0;
    int height = 0;
    int fbWidth = 0;
    int fbHeight = 0;
    const float pixelRatio;

    int availableProcessors = 0;
    size_t totalMemory = 0;

    // Ensure these are initialised last
    std::shared_ptr<mbgl::SQLiteCache> fileCache;
    std::unique_ptr<mbgl::DefaultFileSource> fileSource;
    std::unique_ptr<mbgl::Map> map;
};
}
}

#endif
