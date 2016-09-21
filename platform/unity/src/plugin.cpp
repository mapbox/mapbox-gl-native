#include <mbgl/gl/gl.hpp>
#include <mbgl/map/map.hpp>
#include <mbgl/map/view.hpp>
#include <mbgl/platform/log.hpp>
#include <mbgl/storage/default_file_source.hpp>
#include <mbgl/storage/network_status.hpp>
#include <mbgl/util/default_styles.hpp>
#include <mbgl/util/run_loop.hpp>

#include <IUnityGraphics.h>
#include <IUnityInterface.h>

#include <array>
#include <atomic>
#include <memory>
#include <utility>

#include <stdlib.h>

namespace {

class UnityPluginView;

enum SyncStatus : uint16_t {
    Nothing    = 0,
    Token      = 1 << 1,
    Style      = 1 << 2,
    Zoom       = 1 << 3,
    Bearing    = 1 << 4,
    Pan        = 1 << 5,
    Coordinate = 1 << 6,
    Scale      = 1 << 7,
    Texture    = 1 << 8,
    Everything = 255,
};

struct MapState {
    // Temporary token, will get rotated, do not use in production.
    std::string token = "pk.eyJ1IjoidG1wc2FudG9zIiwiYSI6ImNpdGN0M2I3MjAwNW0yeG1pajg4bWhqY3YifQ.0hqdgg6Hl_lG9ZkTqkB7-w";
    std::string style = mbgl::util::default_styles::orderedStyles[0].url;

    double zoom = 15;
    double bearing = 0;
    double scale = 1;

    mbgl::LatLng center = { 40.7636974, -73.9815443 };
    mbgl::ScreenCoordinate pan = { 0, 0 };
};

struct UnityPluginContext {
    IUnityInterfaces* interfaces = nullptr;
    IUnityGraphics* graphics = nullptr;
    UnityGfxRenderer deviceType = kUnityGfxRendererNull;

    std::unique_ptr<UnityPluginView> view;

    std::unique_ptr<mbgl::util::RunLoop> loop;
    std::unique_ptr<mbgl::DefaultFileSource> fileSource;
    std::unique_ptr<mbgl::Map> map;

    GLuint texture = 0;
    int textureWidth = 0;
    int textureHeight = 0;

    float time = 0;

    // State accessed from the main thread and
    // from the render thread. Needs r/w locking.
    MapState mapState;
    SyncStatus syncStatus = SyncStatus::Everything;
    std::mutex syncLock;
};

// Global plugin context.
UnityPluginContext context;

class UnityPluginView : public mbgl::View {
public:
    float getPixelRatio() const override {
        return pixelRatio;
    }

    std::array<uint16_t, 2> getSize() const override {
        return dimensions;
    }

    std::array<uint16_t, 2> getFramebufferSize() const override {
        return {{ static_cast<uint16_t>(dimensions[0] * pixelRatio),
                  static_cast<uint16_t>(dimensions[1] * pixelRatio) }};
    }

    void resize(uint16_t width, uint16_t height) {
        if (dimensions[0] == width && dimensions[1] == height) {
            return;
        }

        dimensions = {{ width, height }};

        if (fbo != 0) {
            clear();
        }

        MBGL_CHECK_ERROR(glGenRenderbuffers(1, &fboDepthStencil));
        MBGL_CHECK_ERROR(glBindRenderbuffer(GL_RENDERBUFFER, fboDepthStencil));
        MBGL_CHECK_ERROR(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height));
        MBGL_CHECK_ERROR(glBindRenderbuffer(GL_RENDERBUFFER, 0));

        MBGL_CHECK_ERROR(glGenFramebuffers(1, &fbo));
        MBGL_CHECK_ERROR(glBindFramebuffer(GL_FRAMEBUFFER, fbo));

        MBGL_CHECK_ERROR(glFramebufferRenderbuffer(
                GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, fboDepthStencil));

        MBGL_CHECK_ERROR(glBindFramebuffer(GL_FRAMEBUFFER, 0));

        context.map->update(mbgl::Update::Dimensions);
    }

    void clear() {
        MBGL_CHECK_ERROR(glBindFramebuffer(GL_FRAMEBUFFER, 0));
        MBGL_CHECK_ERROR(glDeleteFramebuffers(1, &fbo));
        fbo = 0;

        MBGL_CHECK_ERROR(glDeleteRenderbuffers(1, &fboDepthStencil));
        fboDepthStencil = 0;
    }

    void setTexture(GLuint texture) {
        MBGL_CHECK_ERROR(glBindFramebuffer(GL_FRAMEBUFFER, fbo));
        MBGL_CHECK_ERROR(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0));
    }

    bool swapDirtyState(bool swap){
        std::swap(swap, dirty);

        return swap;
    }

    void invalidate() override {
        dirty = true;
    }

    void activate() override {}
    void deactivate() override {}

private:
    const float pixelRatio = 1.0;
    bool dirty = false;

    GLuint fbo = 0;
    GLuint fboDepthStencil = 0;

    std::array<uint16_t, 2> dimensions = {{ 0, 0 }};
};

void DoSyncState() {
    static SyncStatus syncStatus;
    static MapState mapState;

    {
        std::lock_guard<std::mutex> lock(context.syncLock);

        if (context.syncStatus == SyncStatus::Nothing) {
            return;
        }

        mapState = context.mapState;
        syncStatus = context.syncStatus;

        context.syncStatus = SyncStatus::Nothing;
    }

    if (syncStatus & SyncStatus::Token) {
        context.fileSource->setAccessToken(mapState.token);
    }

    if (syncStatus & SyncStatus::Style) {
        context.map->setStyleURL(mapState.style);
    }

    if (syncStatus & SyncStatus::Zoom || syncStatus & SyncStatus::Coordinate) {
        context.map->setLatLngZoom(mapState.center, mapState.zoom);
    }

    if (syncStatus & SyncStatus::Bearing) {
        context.map->setBearing(mapState.bearing);
    }

    if (syncStatus & SyncStatus::Pan) {
        context.map->moveBy(mapState.pan);
    }

    if (syncStatus & SyncStatus::Scale) {
        context.map->scaleBy(mapState.scale);
    }

    if (syncStatus & SyncStatus::Texture) {
        context.view->invalidate();
    }
}

void DoRendering() {
    if (!context.texture) {
        return;
    }

    MBGL_CHECK_ERROR(glBindTexture(GL_TEXTURE_2D, context.texture));

    context.view->resize(context.textureWidth, context.textureHeight);
    context.view->setTexture(context.texture);

    MBGL_CHECK_ERROR(glViewport(0, 0, context.textureWidth, context.textureHeight));

    if (context.view->swapDirtyState(false)) {
        context.map->render();
    }

    MBGL_CHECK_ERROR(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

void InitializePluginIfNeeded() {
    static bool initialized = false;

    if (initialized) {
        return;
    }

    context.view = std::make_unique<UnityPluginView>();
    context.loop = std::make_unique<mbgl::util::RunLoop>(mbgl::util::RunLoop::Type::New);
    context.fileSource = std::make_unique<mbgl::DefaultFileSource>(":memory:", ".");

    // Sharing the GL context with Unity
    context.map = std::make_unique<mbgl::Map>(*context.view, *context.fileSource,
            mbgl::MapMode::Continuous, mbgl::GLContextMode::Shared);

    initialized = true;

    mbgl::Log::Info(mbgl::Event::General, "Mapbox GL plugin loaded.");
}

} // namespace

// No-op, only here to keep compatibility with the Unity
// native plugin example.
extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API SetTimeFromUnity(float time) {
    context.time = time;
}

// Set the texture we are going to render the map at.
extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API SetTextureFromUnity(void* handle, int width, int height) {
    context.texture = reinterpret_cast<size_t>(handle);
    context.textureWidth = width;
    context.textureHeight = height;

    std::lock_guard<std::mutex> lock(context.syncLock);
    context.syncStatus = static_cast<SyncStatus>(context.syncStatus | SyncStatus::Texture);
}

extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API SetToken(const char *token) {
    std::lock_guard<std::mutex> lock(context.syncLock);
    context.mapState.token = token;
    context.syncStatus = static_cast<SyncStatus>(context.syncStatus | SyncStatus::Token);
}

extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API SetStyle(const char *style) {
    std::lock_guard<std::mutex> lock(context.syncLock);
    context.mapState.style = style;
    context.syncStatus = static_cast<SyncStatus>(context.syncStatus | SyncStatus::Style);
}

extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API SetZoom(double zoom) {
    std::lock_guard<std::mutex> lock(context.syncLock);
    context.mapState.zoom = zoom;
    context.syncStatus = static_cast<SyncStatus>(context.syncStatus | SyncStatus::Zoom);
}

extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API SetBearing(double bearing) {
    std::lock_guard<std::mutex> lock(context.syncLock);
    context.mapState.bearing = bearing;
    context.syncStatus = static_cast<SyncStatus>(context.syncStatus | SyncStatus::Bearing);
}

extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API MoveBy(double dx, double dy) {
    std::lock_guard<std::mutex> lock(context.syncLock);
    context.mapState.pan = { dx, dy };
    context.syncStatus = static_cast<SyncStatus>(context.syncStatus | SyncStatus::Pan);
}

extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API SetCoordinate(double lat, double lng) {
    std::lock_guard<std::mutex> lock(context.syncLock);
    context.mapState.center = { lat, lng };
    context.syncStatus = static_cast<SyncStatus>(context.syncStatus | SyncStatus::Coordinate);
}

extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API ScaleBy(double ds) {
    std::lock_guard<std::mutex> lock(context.syncLock);
    context.mapState.scale = ds;
    context.syncStatus = static_cast<SyncStatus>(context.syncStatus | SyncStatus::Scale);
}

extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API CycleStyle() {
    using namespace mbgl::util;

    static uint8_t current = 1;

    if (++current == default_styles::numOrderedStyles) {
        current = 0;
    }

    default_styles::DefaultStyle newStyle = default_styles::orderedStyles[current];

    {
        std::lock_guard<std::mutex> lock(context.syncLock);
        context.mapState.style = newStyle.url;
        context.syncStatus = static_cast<SyncStatus>(context.syncStatus | SyncStatus::Style);
    }
}

static void UNITY_INTERFACE_API OnGraphicsDeviceEvent(UnityGfxDeviceEventType eventType) {
    if (eventType == kUnityGfxDeviceEventInitialize) {
        context.deviceType = context.graphics->GetRenderer();

        if (context.deviceType != kUnityGfxRendererOpenGL) {
            mbgl::Log::Info(mbgl::Event::General, "GfxRenderer not supported (try -force-opengl).");
        }
    }

    if (eventType == kUnityGfxDeviceEventShutdown) {
        context.deviceType = kUnityGfxRendererNull;
    }
}

extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API UnityPluginLoad(IUnityInterfaces* interfaces) {
    context.interfaces = interfaces;
    context.graphics = interfaces->Get<IUnityGraphics>();
    context.graphics->RegisterDeviceEventCallback(OnGraphicsDeviceEvent);

    OnGraphicsDeviceEvent(kUnityGfxDeviceEventInitialize);
}

extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API UnityPluginUnload() {
    context.graphics->UnregisterDeviceEventCallback(OnGraphicsDeviceEvent);
}

static void UNITY_INTERFACE_API OnRenderEvent(int) {
    InitializePluginIfNeeded();

    DoRendering();
    DoSyncState();

    context.loop->runOnce();
}

extern "C" UnityRenderingEvent UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API GetRenderEventFunc() {
    return OnRenderEvent;
}
