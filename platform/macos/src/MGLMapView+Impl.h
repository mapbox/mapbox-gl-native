#import <mbgl/gfx/renderer_backend.hpp>
#import <mbgl/map/map_observer.hpp>
#import <mbgl/util/image.hpp>

@class MGLMapView;

typedef struct _CGLContextObject* CGLContextObj;

class MGLMapViewImpl : public mbgl::MapObserver {
public:
    static std::unique_ptr<MGLMapViewImpl> Create(MGLMapView*);

    MGLMapViewImpl(MGLMapView*);
    virtual ~MGLMapViewImpl() = default;

    virtual mbgl::gfx::RendererBackend& getRendererBackend() = 0;

    // We need a static image of what was rendered for printing.
    virtual mbgl::PremultipliedImage readStillImage() = 0;

    virtual CGLContextObj getCGLContextObj() {
        return nullptr;
    }

    // mbgl::MapObserver implementation
    void onCameraWillChange(mbgl::MapObserver::CameraChangeMode) override;
    void onCameraIsChanging() override;
    void onCameraDidChange(mbgl::MapObserver::CameraChangeMode) override;
    void onWillStartLoadingMap() override;
    void onDidFinishLoadingMap() override;
    void onDidFailLoadingMap(mbgl::MapLoadError mapError, const std::string& what) override;
    void onWillStartRenderingFrame() override;
    void onDidFinishRenderingFrame(mbgl::MapObserver::RenderMode) override;
    void onWillStartRenderingMap() override;
    void onDidFinishRenderingMap(mbgl::MapObserver::RenderMode) override;
    void onDidFinishLoadingStyle() override;
    void onSourceChanged(mbgl::style::Source& source) override;
    void onDidBecomeIdle() override;
    bool onCanRemoveUnusedStyleImage(const std::string& imageIdentifier) override;

protected:
    /// Cocoa map view that this adapter bridges to.
    __weak MGLMapView *mapView = nullptr;
};
