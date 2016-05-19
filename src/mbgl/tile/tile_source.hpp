#pragma once

#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/optional.hpp>
#include <mbgl/util/chrono.hpp>

#include <memory>
#include <functional>

namespace mbgl {

class GeometryTile;
class AsyncRequest;

class TileSource : private util::noncopyable {
public:
    virtual ~TileSource() = default;
};

class GeometryTileSource : public TileSource {
public:
    virtual ~GeometryTileSource() = default;

    using Callback = std::function<void (std::exception_ptr,
                                         std::unique_ptr<GeometryTile>,
                                         optional<Timestamp> modified,
                                         optional<Timestamp> expires)>;
    /*
     * Monitor the tile held by this object for changes. When the tile is loaded for the first time,
     * or updates, the callback is executed. If an error occurs, the first parameter will be set.
     * Otherwise it will be null. If there is no data for the requested tile, the second parameter
     * will be null.
     *
     * To cease monitoring, release the returned Request.
     */
    virtual std::unique_ptr<AsyncRequest> monitorTile(const Callback&) = 0;
};

} // namespace mbgl
