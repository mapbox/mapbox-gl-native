#pragma once

#include <mbgl/map/map.hpp>
#include <mbgl/map/map_impl.hpp>

namespace mbgl {

class FileSource;

// Non-public version of mbgl::Map that accepts a file source as parameter.
class MapAdapter : public Map {
public:
    explicit MapAdapter(RendererFrontend& frontend,
                        MapObserver& observer,
                        std::shared_ptr<FileSource> fileSource,
                        const MapOptions& options)
        : Map(std::make_unique<Map::Impl>(frontend, observer, std::move(fileSource), options)) {}
};

} // namespace mbgl
