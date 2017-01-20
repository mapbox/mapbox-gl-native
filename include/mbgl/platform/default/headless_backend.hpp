#pragma once

#include <mbgl/gl/implementation.hpp>

#if MBGL_USE_QT
class QGLWidget;
#elif MBGL_USE_CGL
#include <OpenGL/OpenGL.h>
#elif MBGL_USE_GLX
typedef struct _XDisplay Display;
typedef struct __GLXcontextRec* GLXContext;
typedef struct __GLXFBConfigRec* GLXFBConfig;
typedef long unsigned int XID;
typedef XID GLXPbuffer;
#elif MBGL_USE_OSMESA
#include <GL/osmesa.h>
#endif

#include <mbgl/map/backend.hpp>
#include <mbgl/gl/extension.hpp>

#include <memory>
#include <functional>

namespace mbgl {

class HeadlessDisplay;

class HeadlessBackend : public Backend {
public:
    HeadlessBackend();
    HeadlessBackend(std::shared_ptr<HeadlessDisplay>);
    ~HeadlessBackend() override;

    void invalidate() override;
    void activate() override;
    void deactivate() override;
    void notifyMapChange(MapChange) override;

    void setMapChangeCallback(std::function<void(MapChange)>&& cb) { mapChangeCallback = std::move(cb); }

private:
    void activateContext();
    void deactivateContext();

private:
    // Implementation specific functions
    static gl::glProc initializeExtension(const char*);
    void createContext();
    void destroyContext();

    std::shared_ptr<HeadlessDisplay> display;

    bool extensionsLoaded = false;
    bool active = false;

#if MBGL_USE_QT
    QGLWidget* glContext = nullptr;
#endif

#if MBGL_USE_CGL
    CGLContextObj glContext = nullptr;
#endif

#if MBGL_USE_EAGL
    void *glContext = nullptr;
#endif

#if MBGL_USE_GLX
    Display *xDisplay = nullptr;
    GLXFBConfig *fbConfigs = nullptr;
    GLXContext glContext = nullptr;
    GLXPbuffer glxPbuffer = 0;
#endif

#if MBGL_USE_OSMESA
    OSMesaContext glContext = nullptr;
    GLubyte fakeBuffer = 0;
#endif

    std::function<void(MapChange)> mapChangeCallback;

};

} // namespace mbgl
