#pragma once

#include <mbgl/util/image.hpp>
#include <mbgl/util/thread.hpp>
#include <mbgl/util/optional.hpp>
#include <mbgl/util/geo.hpp>

#include <exception>
#include <memory>
#include <string>
#include <vector>
#include <functional>

namespace mbgl {

template<class> class ActorRef;
struct CameraOptions;
class FileSource;
class Size;
class LatLngBounds;

namespace style {
class Style;
} // namespace style

class MapSnapshotter {
public:
    MapSnapshotter(FileSource* fileSource,
                   std::shared_ptr<Scheduler> scheduler,
                   const std::pair<bool, std::string> style,
                   const Size&,
                   const float pixelRatio,
                   const optional<CameraOptions> cameraOptions,
                   const optional<LatLngBounds> region,
                   const optional<std::string> cacheDir = {});

    ~MapSnapshotter();

    void setStyleURL(const std::string& styleURL);
    std::string getStyleURL() const;

    void setStyleJSON(const std::string& styleJSON);
    std::string getStyleJSON() const;

    void setSize(const Size&);
    Size getSize() const;

    void setCameraOptions(const CameraOptions&);
    CameraOptions getCameraOptions() const;

    void setRegion(const LatLngBounds&);
    LatLngBounds getRegion() const;

    using PointForFn = std::function<ScreenCoordinate (const LatLng&)>;
    using LatLngForFn = std::function<LatLng (const ScreenCoordinate&)>;
    using Attributions = std::vector<std::string>;
    using Callback = std::function<void (std::exception_ptr, PremultipliedImage, Attributions, PointForFn, LatLngForFn)>;
    void snapshot(ActorRef<Callback>);

private:
    class Impl;
    std::unique_ptr<util::Thread<Impl>> impl;
};

} // namespace mbgl
