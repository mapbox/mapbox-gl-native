#pragma once

#include <mbgl/util/image.hpp>
#include <mbgl/util/thread.hpp>
#include <mbgl/util/optional.hpp>

#include <exception>
#include <string>
#include <functional>

namespace mbgl {

template<class> class ActorRef;
struct CameraOptions;
class FileSource;
class Size;
class LatLngBounds;

class MapSnapshotter {
public:
    MapSnapshotter(FileSource& fileSource,
                   Scheduler& scheduler,
                   const std::string& styleURL,
                   const Size&,
                   const float pixelRatio,
                   const CameraOptions&,
                   const optional<LatLngBounds> region,
                   const optional<std::string> cacheDir = {});

    ~MapSnapshotter();

    using Callback = std::function<void (std::exception_ptr, PremultipliedImage)>;
    void snapshot(ActorRef<Callback>);

private:
    class Impl;
    std::unique_ptr<util::Thread<Impl>> impl;
};

} // namespace mbgl
