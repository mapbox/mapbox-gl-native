#ifndef MAP_VIEW_HPP
#define MAP_VIEW_HPP

#include <string>

#include <jni.h>

#include <android/native_window.h>

#include <EGL/egl.h>

#include <llmr/llmr.hpp>
// TODO put in namespace llmr::android
class LLMRView;

class NativeMapView {
    friend class LLMRView;

public:
    NativeMapView(JNIEnv* env, jobject obj, std::string default_style_json);
    ~NativeMapView();

    llmr::Map* getMap() const {
        return map;
    }
    ;

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

#endif // MAP_VIEW_HPP
