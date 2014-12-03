#ifndef MBGL_ANDROID_NATIVE_MAP_VIEW
#define MBGL_ANDROID_NATIVE_MAP_VIEW

#include <mbgl/map/map.hpp>
#include <mbgl/map/view.hpp>
#include <mbgl/util/noncopyable.hpp>
#include <string>
#include <jni.h>
#include <android/native_window.h>
#include <EGL/egl.h>

namespace mbgl {
namespace android {

class NativeMapView;

class MBGLView: public mbgl::View, private mbgl::util::noncopyable {
public:
    explicit MBGLView(NativeMapView& nativeView_) : nativeView(nativeView_) {
    }

    void make_active() override;
    void make_inactive() override;

    void swap() override;

    void notify() override;
    void notify_map_change(mbgl::MapChange change, mbgl::timestamp delay = 0) override;

private:
    NativeMapView& nativeView;
};

class NativeMapView : private mbgl::util::noncopyable {
    friend class MBGLView;

public:
    NativeMapView(JNIEnv* env, jobject obj);
    ~NativeMapView();

    mbgl::Map& getMap() {
        return map;
    }

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

    void notifyMapChange();

    void enableFps(bool enable);
    void updateFps();

private:
    EGLConfig chooseConfig(const EGLConfig configs[], EGLint numConfigs);

    void loadExtensions();

private:
    JavaVM* vm = nullptr;
    jobject obj = nullptr;

    ANativeWindow* window = nullptr;

    MBGLView view; // Must be before map
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
