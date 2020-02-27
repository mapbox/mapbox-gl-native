#pragma once

#include <mbgl/util/image.hpp>
#include <mbgl/util/optional.hpp>
#include <mbgl/util/geo.hpp>

#include <exception>
#include <memory>
#include <string>
#include <vector>
#include <functional>
namespace mbgl {

struct CameraOptions;
class LatLngBounds;
class ResourceOptions;

namespace style {
class Style;
} // namespace style

class MapSnapshotter {
public:
    MapSnapshotter(std::pair<bool, std::string> style,
                   Size size,
                   float pixelRatio,
                   optional<CameraOptions> cameraOptions,
                   optional<LatLngBounds> region,
                   optional<std::string> localFontFamily,
                   const ResourceOptions&);

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

    style::Style& getStyle();
    const style::Style& getStyle() const;

    using PointForFn = std::function<ScreenCoordinate (const LatLng&)>;
    using LatLngForFn = std::function<LatLng (const ScreenCoordinate&)>;
    using Attributions = std::vector<std::string>;
    using Callback = std::function<void (std::exception_ptr, PremultipliedImage, Attributions, PointForFn, LatLngForFn)>;
    void snapshot(Callback);

private:
    class Impl;
    std::unique_ptr<Impl> impl;
};

} // namespace mbgl
