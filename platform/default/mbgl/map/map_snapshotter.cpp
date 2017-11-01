#include <mbgl/map/map_snapshotter.hpp>

#include <mbgl/actor/actor_ref.hpp>
#include <mbgl/gl/headless_frontend.hpp>
#include <mbgl/map/map.hpp>
#include <mbgl/storage/file_source.hpp>
#include <mbgl/style/style.hpp>
#include <mbgl/util/event.hpp>

namespace mbgl {

class MapSnapshotter::Impl {
public:
    Impl(FileSource&,
         Scheduler&,
         const std::string& styleURL,
         const Size&,
         const float pixelRatio,
         const CameraOptions&,
         const optional<LatLngBounds> region,
         const optional<std::string> programCacheDir);

    void setStyleURL(std::string styleURL);
    std::string getStyleURL() const;

    void setSize(Size);
    Size getSize() const;

    void setCameraOptions(CameraOptions);
    CameraOptions getCameraOptions() const;

    void setRegion(LatLngBounds);
    LatLngBounds getRegion() const;

    void snapshot(ActorRef<MapSnapshotter::Callback>);

private:
    HeadlessFrontend frontend;
    Map map;
};

MapSnapshotter::Impl::Impl(FileSource& fileSource,
           Scheduler& scheduler,
           const std::string& styleURL,
           const Size& size,
           const float pixelRatio,
           const CameraOptions& cameraOptions,
           const optional<LatLngBounds> region,
           const optional<std::string> programCacheDir)
    : frontend(size, pixelRatio, fileSource, scheduler, programCacheDir)
    , map(frontend, MapObserver::nullObserver(), size, pixelRatio, fileSource, scheduler, MapMode::Still) {

    map.getStyle().loadURL(styleURL);

    map.jumpTo(cameraOptions);

    // Set region, if specified
    if (region) {
        this->setRegion(*region);
    }
}

void MapSnapshotter::Impl::snapshot(ActorRef<MapSnapshotter::Callback> callback) {
    map.renderStill([this, callback = std::move(callback)] (std::exception_ptr error) mutable {
        callback.invoke(&MapSnapshotter::Callback::operator(), error, error ? PremultipliedImage() : frontend.readStillImage());
    });
}

void MapSnapshotter::Impl::setStyleURL(std::string styleURL) {
    map.getStyle().loadURL(styleURL);
}

std::string MapSnapshotter::Impl::getStyleURL() const {
    return map.getStyle().getURL();
}

void MapSnapshotter::Impl::setSize(Size size) {
    map.setSize(size);
}

Size MapSnapshotter::Impl::getSize() const {
    return map.getSize();
}

void MapSnapshotter::Impl::setCameraOptions(CameraOptions cameraOptions) {
    map.jumpTo(cameraOptions);
}

CameraOptions MapSnapshotter::Impl::getCameraOptions() const {
    EdgeInsets insets;
    return map.getCameraOptions(insets);
}

void MapSnapshotter::Impl::setRegion(LatLngBounds region) {
    mbgl::EdgeInsets insets = { 0, 0, 0, 0 };
    std::vector<LatLng> latLngs = { region.southwest(), region.northeast() };
    map.jumpTo(map.cameraForLatLngs(latLngs, insets));
}

LatLngBounds MapSnapshotter::Impl::getRegion() const {
    return map.latLngBoundsForCamera(getCameraOptions());
}

MapSnapshotter::MapSnapshotter(FileSource& fileSource,
                               Scheduler& scheduler,
                               const std::string& styleURL,
                               const Size& size,
                               const float pixelRatio,
                               const CameraOptions& cameraOptions,
                               const optional<LatLngBounds> region,
                               const optional<std::string> programCacheDir)
   : impl(std::make_unique<util::Thread<MapSnapshotter::Impl>>("Map Snapshotter", fileSource, scheduler, styleURL, size, pixelRatio, cameraOptions, region, programCacheDir)) {
}

MapSnapshotter::~MapSnapshotter() = default;

void MapSnapshotter::snapshot(ActorRef<MapSnapshotter::Callback> callback) {
    impl->actor().invoke(&Impl::snapshot, std::move(callback));
}

void MapSnapshotter::setStyleURL(const std::string& styleURL) {
    impl->actor().invoke(&Impl::setStyleURL, styleURL);
}

std::string MapSnapshotter::getStyleURL() const {
    return impl->actor().ask(&Impl::getStyleURL).get();
}

void MapSnapshotter::setSize(const Size& size) {
    impl->actor().invoke(&Impl::setSize, size);
}

Size MapSnapshotter::getSize() const {
    return impl->actor().ask(&Impl::getSize).get();
}

void MapSnapshotter::setCameraOptions(const CameraOptions& options) {
    impl->actor().invoke(&Impl::setCameraOptions, options);
}

CameraOptions MapSnapshotter::getCameraOptions() const {
    return impl->actor().ask(&Impl::getCameraOptions).get();
}

void MapSnapshotter::setRegion(const LatLngBounds& bounds) {
    impl->actor().invoke(&Impl::setRegion, std::move(bounds));
}

LatLngBounds MapSnapshotter::getRegion() const {
    return impl->actor().ask(&Impl::getRegion).get();
}

} // namespace mbgl
