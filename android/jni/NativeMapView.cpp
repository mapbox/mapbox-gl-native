#include <stdio.h>

#include <GLES2/gl2.h>

#include <llmr/platform/platform.hpp>

#include "log.h"

#include "NativeMapView.hpp"

void log_egl_string(EGLDisplay display, EGLint name, const char* label) {
	const char* str = eglQueryString(display, name);
	if (str == nullptr) {
		ERROR("eglQueryString(%d) returned error %d", name, eglGetError());
	} else {
		INFO("EGL %s: %s", label, str);
	}
}

void log_gl_string(GLenum name, const char* label) {
	const GLubyte* str = glGetString(name);
	if (str == nullptr) {
		ERROR("glGetString(%d) returned error %d", name, glGetError());
	} else {
		INFO("GL %s: %s", label, str);
	}
}

NativeMapView::NativeMapView(std::string default_style_json) : default_style_json(default_style_json) {
	VERBOSE("NativeMapView constructor");

	freopen("/sdcard/stdout.txt", "w", stdout); // NOTE: can't use <cstdio> till NDK fix the stdout macro bug
	freopen("/sdcard/stderr.txt", "w", stderr);

	view = new LLMRView(this);
	map = new llmr::Map(*view);

	// TODO move out of here
	map->setStyleJSON(default_style_json);
}

NativeMapView::~NativeMapView() {
	VERBOSE("NativeMapView destructor");
	terminateContext();

	delete map;
	map = nullptr;

	delete view;
	view = nullptr;
}

bool NativeMapView::initializeContext(ANativeWindow* window) {
	VERBOSE("NativeMapView initializeContext");

	ASSERT(this->window == nullptr);
	ASSERT(window != nullptr);
	this->window = window;

	ASSERT(display == EGL_NO_DISPLAY);
	ASSERT(surface == EGL_NO_SURFACE);
	ASSERT(context == EGL_NO_CONTEXT);

	display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	if (display == EGL_NO_DISPLAY) {
		ERROR("eglGetDisplay() returned error %d", eglGetError());
		terminateContext();
		return false;
	}

	EGLint major, minor;
	if (!eglInitialize(display, &major, &minor)) {
		ERROR("eglInitialize() returned error %d", eglGetError());
		terminateContext();
		return false;
	}
	if ((major <= 1) && (minor < 3)) {
		ERROR("EGL version is too low, need 1.3, got %d.%d", major, minor);
		terminateContext();
		return false;
	}

	log_egl_string(display, EGL_VENDOR, "Vendor");
	log_egl_string(display, EGL_VERSION, "Version");
	log_egl_string(display, EGL_CLIENT_APIS, "Client APIs");
	log_egl_string(display, EGL_EXTENSIONS, "Client Extensions");
	if (eglQueryString(EGL_NO_DISPLAY, EGL_EXTENSIONS) != nullptr) {
		log_egl_string(EGL_NO_DISPLAY, EGL_EXTENSIONS, "Display Extensions");
	}

	const EGLint config_attribs[] = {
		EGL_CONFIG_CAVEAT, EGL_NONE,
		EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
		EGL_COLOR_BUFFER_TYPE, EGL_RGB_BUFFER,
		EGL_RED_SIZE, 8,
		EGL_GREEN_SIZE, 8,
		EGL_BLUE_SIZE, 8,
		EGL_DEPTH_SIZE, 16,
		EGL_STENCIL_SIZE, 8,
		EGL_NONE
	};
	EGLConfig config;
	EGLint num_configs;
	if (!eglChooseConfig(display, config_attribs, &config, 1, &num_configs)) {
		ERROR("eglChooseConfig() returned error %d", eglGetError());
		terminateContext();
		return false;
	}
	if (num_configs < 1) {
		ERROR("eglChooseConfig() returned no configs");
		terminateContext();
		return false;
	}

	EGLint format;
	if (!eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format)) {
		ERROR("eglGetConfigAttrib() returned error %d", eglGetError());
		terminateContext();
		return false;
	}
	DEBUG("Chosen window format is %d", format);

	ANativeWindow_setBuffersGeometry(window, 0, 0, format);

	const EGLint surface_attribs[] = {
		EGL_NONE
	};
	surface = eglCreateWindowSurface(display, config, window, surface_attribs);
	if (surface == EGL_NO_SURFACE) {
		ERROR("eglCreateWindowSurface() returned error %d", eglGetError());
		terminateContext();
		return false;
	}

	EGLint width, height;
	if (!eglQuerySurface(display, surface, EGL_WIDTH, &width) ||
		!eglQuerySurface(display, surface, EGL_HEIGHT, &height)) {
		ERROR("eglQuerySurface() returned error %d", eglGetError());
		terminateContext();
		return false;
	}

	const EGLint context_attribs[] = {
		EGL_CONTEXT_CLIENT_VERSION, 2,
		EGL_NONE
	};
	context = eglCreateContext(display, config, EGL_NO_CONTEXT, context_attribs);
	if (context == EGL_NO_CONTEXT) {
		ERROR("eglCreateContext() returned error %d", eglGetError());
		terminateContext();
		return false;
	}

	if (!eglMakeCurrent(display, surface, surface, context)) {
		ERROR("eglMakeCurrent() returned error %d", eglGetError());
		terminateContext();
		return false;
	}

	log_gl_string(GL_VENDOR, "Vendor");
	log_gl_string(GL_RENDERER, "Renderer");
	log_gl_string(GL_VERSION, "Version");
	log_gl_string(GL_SHADING_LANGUAGE_VERSION, "SL Version");
	log_gl_string(GL_EXTENSIONS, "Extensions");

	map->resize(width, height);

	// TODO: need to sort out which threads to which make current
	if (!eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT)) {
		ERROR("eglMakeCurrent(EGL_NO_CONTEXT) returned error %d", eglGetError());
		terminateContext();
		return false;
	}

	INFO("Context initialized");

	map->start();

	return true;
}

void NativeMapView::terminateContext() {
	VERBOSE("NativeMapView terminateContext");

	map->cleanup();
	// TODO: there is a bug when you double tap home to go app switcher, as map is black if you immediately switch to the map again
	// TODO: this is in the onPause/onResume path
	// TODO: the bug causes an GL_INVALID_VALUE with glDelteProgram (I think due to context being deleted first)
	// TODO: we need to free resources before we terminate
	// TODO: but cause terminate and stop is async they try to do stuff with no context and crash!
	//map->stop();

	if (display != EGL_NO_DISPLAY) {
		eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);

		if (context != EGL_NO_CONTEXT) {
			eglDestroyContext(display, context);
		}

		if (surface != EGL_NO_SURFACE) {
			eglDestroySurface(display, surface);
		}

		eglTerminate(display);
	}

	context = EGL_NO_CONTEXT;
	surface = EGL_NO_SURFACE;
	display = EGL_NO_DISPLAY;

	if (window != nullptr) {
		ANativeWindow_release(window);
		window = nullptr;
	}
}

void NativeMapView::start() {
	VERBOSE("NativeMapView start");
	if ((display != EGL_NO_DISPLAY) && (surface != EGL_NO_SURFACE) && (context != EGL_NO_CONTEXT)) {
		map->start();
	}
}

void NativeMapView::stop() {
	VERBOSE("NativeMapView stop");
	if ((display != EGL_NO_DISPLAY) && (surface != EGL_NO_SURFACE) && (context != EGL_NO_CONTEXT)) {
		map->stop();
	}
}

void NativeMapView::updateAndWait() {
	VERBOSE("NativeMapView updateAndWait");
	map->update();
	//while(map->needsSwap()) {
		// TODO: this is not working!!!
		// TODO: this is not very efficient
		// nop
	//}
}

void LLMRView::make_active()
{
	VERBOSE("LLMRView make_active");
	// TODO how do we undo this? If thread is different from init thread?
	if ((nativeView->display != EGL_NO_DISPLAY) && (nativeView->surface != EGL_NO_SURFACE) && (nativeView->context != EGL_NO_CONTEXT)) {
		if (!eglMakeCurrent(nativeView->display, nativeView->surface, nativeView->surface, nativeView->context)) {
			ERROR("eglMakeCurrent() returned error %d", eglGetError());
		}
	}
}

void LLMRView::swap()
{
	VERBOSE("LLMRView swap");
	if (map->needsSwap() && (nativeView->display != EGL_NO_DISPLAY) && (nativeView->surface != EGL_NO_SURFACE)) {
		if (!eglSwapBuffers(nativeView->display, nativeView->surface)) {
			ERROR("eglSwapBuffers() returned error %d", eglGetError());
		}
		map->swapped();
	}
}

void llmr::platform::notify_map_change() {
    DEBUG("notify_map_change() called");
    // TODO is only one instance of the map allowed?
}
