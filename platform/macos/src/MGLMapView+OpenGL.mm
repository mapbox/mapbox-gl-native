#import "MGLMapView+OpenGL.h"
#import "MGLOpenGLLayer.h"

#include <mbgl/gl/renderable_resource.hpp>

#import <OpenGL/gl.h>

class MGLMapViewOpenGLRenderableResource final : public mbgl::gl::RenderableResource {
public:
    MGLMapViewOpenGLRenderableResource(MGLMapViewOpenGLImpl& backend_) : backend(backend_) {
    }

    void bind() override {
        backend.restoreFramebufferBinding();
    }

private:
    MGLMapViewOpenGLImpl& backend;

public:
    // The current framebuffer of the NSOpenGLLayer we are painting to.
    GLint fbo = 0;

    // The reference counted count of activation calls
    NSUInteger activationCount = 0;
};

MGLMapViewOpenGLImpl::MGLMapViewOpenGLImpl(MGLMapView* nativeView_)
    : MGLMapViewImpl(nativeView_),
      mbgl::gl::RendererBackend(mbgl::gfx::ContextMode::Unique),
      mbgl::gfx::Renderable(mapView.framebufferSize,
                            std::make_unique<MGLMapViewOpenGLRenderableResource>(*this)) {

    // Install the OpenGL layer. Interface Builder’s synchronous drawing means
    // we can’t display a map, so don’t even bother to have a map layer.
    mapView.layer =
        mapView.isTargetingInterfaceBuilder ? [CALayer layer] : [MGLOpenGLLayer layer];
}

mbgl::gl::ProcAddress MGLMapViewOpenGLImpl::getExtensionFunctionPointer(const char* name) {
    static CFBundleRef framework = CFBundleGetBundleWithIdentifier(CFSTR("com.apple.opengl"));
    if (!framework) {
        throw std::runtime_error("Failed to load OpenGL framework.");
    }

    return reinterpret_cast<mbgl::gl::ProcAddress>(CFBundleGetFunctionPointerForName(
        framework, (__bridge CFStringRef)[NSString stringWithUTF8String:name]));
}

void MGLMapViewOpenGLImpl::activate() {
    auto& resource = getResource<MGLMapViewOpenGLRenderableResource>();
    if (resource.activationCount++) {
        return;
    }

    MGLOpenGLLayer* layer = (MGLOpenGLLayer*)mapView.layer;
    [layer.openGLContext makeCurrentContext];
}

void MGLMapViewOpenGLImpl::deactivate() {
    auto& resource = getResource<MGLMapViewOpenGLRenderableResource>();
    if (--resource.activationCount) {
        return;
    }

    [NSOpenGLContext clearCurrentContext];
}

void MGLMapViewOpenGLImpl::updateAssumedState() {
    auto& resource = getResource<MGLMapViewOpenGLRenderableResource>();
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &resource.fbo);
    assumeFramebufferBinding(resource.fbo);
    assumeViewport(0, 0, mapView.framebufferSize);
}

void MGLMapViewOpenGLImpl::restoreFramebufferBinding() {
    auto& resource = getResource<MGLMapViewOpenGLRenderableResource>();
    setFramebufferBinding(resource.fbo);
    setViewport(0, 0, mapView.framebufferSize);
}

mbgl::PremultipliedImage MGLMapViewOpenGLImpl::readStillImage() {
    return readFramebuffer(mapView.framebufferSize);
}

CGLContextObj MGLMapViewOpenGLImpl::getCGLContextObj() {
    MGLOpenGLLayer* layer = (MGLOpenGLLayer*)mapView.layer;
    return layer.openGLContext.CGLContextObj;
}
