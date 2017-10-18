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
        mbgl::EdgeInsets insets = { 0, 0, 0, 0 };
        std::vector<LatLng> latLngs = { region->southwest(), region->northeast() };
        map.jumpTo(map.cameraForLatLngs(latLngs, insets));
    }
}

void MapSnapshotter::Impl::snapshot(ActorRef<MapSnapshotter::Callback> callback) {
    map.renderStill([this, callback = std::move(callback)] (std::exception_ptr error) mutable {
        callback.invoke(&MapSnapshotter::Callback::operator(), error, error ? PremultipliedImage() : frontend.readStillImage());
    });
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

} // namespace mbgl
