#ifndef MAP_VIEW_HPP
#define MAP_VIEW_HPP

#include <string>

#include <jni.h>

#include <android/native_window.h>

#include <EGL/egl.h>

#include <llmr/llmr.hpp>

namespace llmr {
namespace android {

extern jmethodID on_map_changed_id;

extern jclass lon_lat_class;
extern jmethodID lon_lat_constructor_id;
extern jfieldID lon_lat_lon_id;
extern jfieldID lon_lat_lat_id;

extern jclass lon_lat_zoom_class;
extern jmethodID lon_lat_zoom_constructor_id;
extern jfieldID lon_lat_zoom_lon_id;
extern jfieldID lon_lat_zoom_lat_id;
extern jfieldID lon_lat_zoom_zoom_id;

extern jclass runtime_exception_class;

extern jmethodID set_to_array_id;

extern jclass tree_set_class;
extern jmethodID tree_set_constructor_id;
extern jmethodID tree_set_add_id;

class LLMRView;

class NativeMapView {
    friend class LLMRView;

public:
    NativeMapView(JNIEnv* env, jobject obj, std::string default_style_json);
    ~NativeMapView();

    llmr::Map* getMap() const {
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

    void notifyMapChange();

private:
    EGLConfig chooseConfig(const EGLConfig configs[], EGLint num_configs,
            bool use565) const;

private:
    JavaVM* vm = nullptr;
    jobject obj = nullptr;

    ANativeWindow* window = nullptr;

    llmr::Map* map = nullptr;
    LLMRView* view = nullptr;

    EGLDisplay display = EGL_NO_DISPLAY;
    EGLSurface surface = EGL_NO_SURFACE;
    EGLContext context = EGL_NO_CONTEXT;

    EGLConfig config = nullptr;
    EGLint format = -1;

    std::string default_style_json;
};

class LLMRView: public llmr::View {
public:
    LLMRView(NativeMapView* nativeView) :
            nativeView(nativeView) {
    }
    virtual ~LLMRView() {
    }

    void make_active() override;
    void make_inactive() override;

    void swap() override;

    void notify_map_change() override;

private:
    NativeMapView* nativeView = nullptr;
};

} // namespace android
} // namespace llmr

#endif // MAP_VIEW_HPP
