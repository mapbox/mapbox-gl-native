#ifndef MBGL_ANDROID_NATIVE_MAP_VIEW
#define MBGL_ANDROID_NATIVE_MAP_VIEW

#include <mbgl/map/map.hpp>
#include <mbgl/map/view.hpp>
#include <mbgl/util/noncopyable.hpp>
#include <mbgl/storage/caching_http_file_source.hpp>

#include <string>
#include <jni.h>
#include <android/native_window.h>
#include <EGL/egl.h>

namespace mbgl {
namespace android {

class NativeMapView : public mbgl::View, private mbgl::util::noncopyable {
public:
    NativeMapView(JNIEnv* env, jobject obj);
    virtual ~NativeMapView();

    void activate() override;
    void deactivate() override;

    void swap() override;

    void notify() override;
    void notifyMapChange(mbgl::MapChange change, mbgl::timestamp delay) override;

    mbgl::Map& getMap();
    mbgl::CachingHTTPFileSource& getFileSource();

    bool initializeDisplay();
    void terminateDisplay();

    bool initializeContext();
    void terminateContext();

    bool createSurface(ANativeWindow* window);
    void destroySurface();

    void start();
    void stop();

    void resume();
    void pause(bool waitForPause = false);

    void enableFps(bool enable);
    void updateFps();

private:
    EGLConfig chooseConfig(const EGLConfig configs[], EGLint numConfigs);

    void loadExtensions();

private:
    JavaVM* vm = nullptr;
    jobject obj = nullptr;

    ANativeWindow* window = nullptr;

    mbgl::CachingHTTPFileSource fileSource;
    mbgl::Map map;

    EGLDisplay display = EGL_NO_DISPLAY;
    EGLSurface surface = EGL_NO_SURFACE;
    EGLContext context = EGL_NO_CONTEXT;

    EGLConfig config = nullptr;
    EGLint format = -1;

    std::string styleUrl;
    std::string apiKey;

    bool firstTime = false;

    bool usingDepth24 = false;

    bool fpsEnabled = false;
    double fps = 0.0;
};

}
}

#endif
