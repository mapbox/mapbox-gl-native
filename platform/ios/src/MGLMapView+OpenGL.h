#import "MGLMapView+Impl.h"
#import "MGLMapView_Private.h"

#include <mbgl/gfx/renderable.hpp>
#include <mbgl/gl/renderer_backend.hpp>

@class MGLMapViewImplDelegate;

/// Adapter responsible for bridging calls from mbgl to MGLMapView and Cocoa.
class MGLMapViewOpenGLImpl final : public MGLMapViewImpl,
                                   public mbgl::gl::RendererBackend,
                                   public mbgl::gfx::Renderable {
public:
    MGLMapViewOpenGLImpl(MGLMapView*);
    ~MGLMapViewOpenGLImpl() override;

public:
    void restoreFramebufferBinding();

#ifdef MGL_RECREATE_GL_IN_AN_EMERGENCY
private:
    void emergencyRecreateGL();
#endif

    // Implementation of mbgl::gfx::RendererBackend
public:
    mbgl::gfx::Renderable& getDefaultRenderable() override {
        return *this;
    }

private:
    void activate() override;
    void deactivate() override;
    // End implementation of mbgl::gfx::RendererBackend

    // Implementation of mbgl::gl::RendererBackend
public:
    void updateAssumedState() override;

private:
    mbgl::gl::ProcAddress getExtensionFunctionPointer(const char* name) override;
    // End implementation of mbgl::gl::Rendererbackend

    // Implementation of MGLMapViewImpl
public:
    mbgl::gfx::RendererBackend& getRendererBackend() override {
        return *this;
    }

    EAGLContext* getEAGLContext() override;
    void setOpaque(bool) override;
    void display() override;
    void setPresentsWithTransaction(bool) override;
    void createView() override;
    UIView* getView() override;
    void deleteView() override;
    UIImage* snapshot() override;
    void layoutChanged() override;
    // End implementation of MGLMapViewImpl
};
