#include <mbgl/map/map_snapshotter.hpp>

#include <mbgl/actor/actor_ref.hpp>
#include <mbgl/gfx/headless_frontend.hpp>
#include <mbgl/map/map.hpp>
#include <mbgl/map/map_options.hpp>
#include <mbgl/map/transform_state.hpp>
#include <mbgl/storage/resource_options.hpp>
#include <mbgl/style/style.hpp>
#include <mbgl/util/event.hpp>
#include <mbgl/map/transform.hpp>

namespace mbgl {

class MapSnapshotter::Impl {
public:
    Impl(const std::pair<bool, std::string> style,
         const Size&,
         const float pixelRatio,
         const optional<CameraOptions> cameraOptions,
         const optional<LatLngBounds> region,
         const optional<std::string> localFontFamily,
         const ResourceOptions& resourceOptions);

    void setStyleURL(std::string styleURL);
    std::string getStyleURL() const;

    void setStyleJSON(std::string styleJSON);
    std::string getStyleJSON() const;

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

MapSnapshotter::Impl::Impl(const std::pair<bool, std::string> style,
                           const Size& size,
                           const float pixelRatio,
                           const optional<CameraOptions> cameraOptions,
                           const optional<LatLngBounds> region,
                           const optional<std::string> localFontFamily,
                           const ResourceOptions& resourceOptions)
    : frontend(
          size, pixelRatio, gfx::HeadlessBackend::SwapBehaviour::NoFlush, gfx::ContextMode::Unique, localFontFamily),
      map(frontend,
          MapObserver::nullObserver(),
          MapOptions().withMapMode(MapMode::Static).withSize(size).withPixelRatio(pixelRatio),
          resourceOptions) {
    if (style.first) {
        map.getStyle().loadJSON(style.second);
    } else {
        map.getStyle().loadURL(style.second);
    }

    if (cameraOptions) {
        map.jumpTo(*cameraOptions);
    }

    // Set region, if specified
    if (region) {
        this->setRegion(*region);
    }
}

void MapSnapshotter::Impl::snapshot(ActorRef<MapSnapshotter::Callback> callback) {
    map.renderStill([this, callback = std::move(callback)] (std::exception_ptr error) {

        // Create lambda that captures the current transform state
        // and can be used to translate for geographic to screen
        // coordinates
        assert (frontend.getTransformState());
        PointForFn pointForFn { [=, center = *map.getCameraOptions().center, transformState = *frontend.getTransformState()] (const LatLng& latLng) {
            LatLng unwrappedLatLng = latLng.wrapped();
            unwrappedLatLng.unwrapForShortestPath(center);
            Transform transform { transformState };
            return transform.latLngToScreenCoordinate(unwrappedLatLng);
        }};

        // Create lambda that captures the current transform state
        // and can be used to translate for geographic to screen
        // coordinates
        assert (frontend.getTransformState());
        LatLngForFn latLngForFn { [=, transformState = *frontend.getTransformState()] (const ScreenCoordinate& screenCoordinate) {
            Transform transform { transformState };
            return transform.screenCoordinateToLatLng(screenCoordinate);
        }};

        // Collect all source attributions
        std::vector<std::string> attributions;
        for (auto source : map.getStyle().getSources()) {
            auto attribution = source->getAttribution();
            if (attribution) {
                attributions.push_back(*attribution);
            }
        }

        // Invoke callback
        callback.invoke(
                &MapSnapshotter::Callback::operator(),
                error,
                error ? PremultipliedImage() : frontend.readStillImage(),
                std::move(attributions),
                std::move(pointForFn),
                std::move(latLngForFn)
        );
    });
}

void MapSnapshotter::Impl::setStyleURL(std::string styleURL) {
    map.getStyle().loadURL(styleURL);
}

std::string MapSnapshotter::Impl::getStyleURL() const {
    return map.getStyle().getURL();
}

void MapSnapshotter::Impl::setStyleJSON(std::string styleJSON) {
    map.getStyle().loadJSON(styleJSON);
}

std::string MapSnapshotter::Impl::getStyleJSON() const {
   return map.getStyle().getJSON();
}

void MapSnapshotter::Impl::setSize(Size size) {
    map.setSize(size);
    frontend.setSize(size);
}

Size MapSnapshotter::Impl::getSize() const {
    return map.getMapOptions().size();
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

MapSnapshotter::MapSnapshotter(const std::pair<bool, std::string> style,
                               const Size& size,
                               const float pixelRatio,
                               const optional<CameraOptions> cameraOptions,
                               const optional<LatLngBounds> region,
                               const optional<std::string> localFontFamily,
                               const ResourceOptions& resourceOptions)
   : impl(std::make_unique<util::Thread<MapSnapshotter::Impl>>(
       "Map Snapshotter", style, size, pixelRatio, cameraOptions,
       region, localFontFamily, resourceOptions.clone())) {}

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

void MapSnapshotter::setStyleJSON(const std::string& styleJSON) {
    impl->actor().invoke(&Impl::setStyleJSON, styleJSON);
}

std::string MapSnapshotter::getStyleJSON() const {
    return impl->actor().ask(&Impl::getStyleJSON).get();
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
