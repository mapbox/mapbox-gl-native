#ifndef MBGL_ANDROID_NATIVE_MAP_VIEW
#define MBGL_ANDROID_NATIVE_MAP_VIEW

#include <mbgl/map/view.hpp>
#include <string>
#include <jni.h>
#include <android/native_window.h>
#include <EGL/egl.h>

namespace mbgl {
namespace android {

class MBGLView;

class NativeMapView {
    friend class MBGLView;

public:
    NativeMapView(JNIEnv* env, jobject obj);
    ~NativeMapView();

    mbgl::Map* getMap() const {
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
    void pause(bool wait_for_pause = false);

    void notifyMapChange();

private:
    EGLConfig chooseConfig(const EGLConfig configs[], EGLint num_configs,
            bool use565) const;

    void loadExtensions();

private:
    JavaVM* vm = nullptr;
    jobject obj = nullptr;

    ANativeWindow* window = nullptr;

    mbgl::Map* map = nullptr;
    MBGLView* view = nullptr;

    EGLDisplay display = EGL_NO_DISPLAY;
    EGLSurface surface = EGL_NO_SURFACE;
    EGLContext context = EGL_NO_CONTEXT;

    EGLConfig config = nullptr;
    EGLint format = -1;

    std::string style_url;
    std::string api_key;

    bool first_time = false;
};

class MBGLView: public mbgl::View {
public:
    MBGLView(NativeMapView* nativeView_) :
            nativeView(nativeView_) {
    }
    virtual ~MBGLView() {
    }

    void make_active() override;
    void make_inactive() override;

    void swap() override;

    void notify() override;
    void notify_map_change(mbgl::MapChange change, mbgl::timestamp delay = 0) override;

private:
    NativeMapView* nativeView = nullptr;
};

}
}

#endif
