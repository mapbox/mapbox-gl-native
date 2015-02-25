#ifndef MBGL_ANDROID_NATIVE_MAP_VIEW
#define MBGL_ANDROID_NATIVE_MAP_VIEW

#include <mbgl/map/map.hpp>
#include <mbgl/map/view.hpp>
#include <mbgl/util/noncopyable.hpp>
#include <mbgl/storage/default/sqlite_cache.hpp>
#include <mbgl/storage/default_file_source.hpp>

#include <string>
#include <jni.h>
#include <android/native_window.h>
#include <EGL/egl.h>

namespace mbgl {
namespace android {

class NativeMapView : public mbgl::View {
public:
    NativeMapView(JNIEnv *env, jobject obj);
    virtual ~NativeMapView();

    void initialize(Map *) override {};

    void activate() override;
    void deactivate() override;

    void swap() override;

    void notifyMapChange(mbgl::MapChange change, std::chrono::steady_clock::duration delay = std::chrono::steady_clock::duration::zero()) override;

    mbgl::Map &getMap();
    mbgl::DefaultFileSource &getFileSource();

    void initializeDisplay();
    void terminateDisplay();

    void initializeContext();
    void terminateContext();

    void createSurface(ANativeWindow *window);
    void destroySurface();

    void enableFps(bool enable);
    void updateFps();

    void resize(uint16_t width, uint16_t height, float ratio, uint16_t fbWidth, uint16_t fbHeight);

private:
    EGLConfig chooseConfig(const EGLConfig configs[], EGLint numConfigs);

    static void loadExtensions();

    bool inEmulator();

private:
    JavaVM *vm = nullptr;
    jobject obj = nullptr;

    ANativeWindow *window = nullptr;

    mbgl::SQLiteCache fileCache;
    mbgl::DefaultFileSource fileSource;
    mbgl::Map map;

    EGLDisplay display = EGL_NO_DISPLAY;
    EGLSurface surface = EGL_NO_SURFACE;
    EGLContext context = EGL_NO_CONTEXT;

    EGLConfig config = nullptr;
    EGLint format = -1;

    std::string styleUrl;
    std::string apiKey;

    bool fpsEnabled = false;
    double fps = 0.0;
};
}
}

#endif
