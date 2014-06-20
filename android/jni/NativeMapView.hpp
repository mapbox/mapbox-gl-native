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
    NativeMapView(ANativeWindow* window, std::string default_style_json);
    ~NativeMapView();

    bool initialize();

    void resize(int width, int height);
    void redraw();

    void pause();
    void resume();
    void stop();

private:
    void destroy();

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
