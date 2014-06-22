#ifndef MAP_VIEW_HPP
#define MAP_VIEW_HPP

#include <string>

#include <android/native_window.h>

#include <EGL/egl.h>

#include <llmr/llmr.hpp>

class LLMRView;

class NativeMapView {
	friend class LLMRView;

public:
    NativeMapView(std::string default_style_json);
    ~NativeMapView();

    llmr::Map* getMap() { return map; };

    bool initializeContext(ANativeWindow* window);
    void terminateContext();

    void start();
    void stop();

    void updateAndWait();

private:
    int chooseConfig(const EGLConfig configs[], EGLint num_configs);

private:
    llmr::Map* map = nullptr;
    LLMRView* view = nullptr;

    ANativeWindow* window = nullptr;

    EGLDisplay display = EGL_NO_DISPLAY;
    EGLSurface surface = EGL_NO_SURFACE;
    EGLContext context = EGL_NO_CONTEXT;

    std::string default_style_json;
};

class LLMRView : public llmr::View
{
public:
	LLMRView(NativeMapView* nativeView) : nativeView(nativeView) {}
	virtual ~LLMRView() {}

	//void notify_map_change();

	void make_active() override;

	void swap() override;

private:
	NativeMapView* nativeView = nullptr;
};

#endif // MAP_VIEW_HPP
