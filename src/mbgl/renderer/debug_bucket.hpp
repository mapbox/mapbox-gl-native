#pragma once

#include <mbgl/map/mode.hpp>
#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/chrono.hpp>
#include <mbgl/util/optional.hpp>

namespace mbgl {

class OverscaledTileID;
class DebugRenderable;

namespace gl {
class ObjectStore;
}

class DebugBucket : private util::noncopyable {
public:
    DebugBucket(const OverscaledTileID& id,
                bool isRenderable,
                bool isComplete,
                optional<Timestamp> modified,
                optional<Timestamp> expires,
                MapDebugOptions);
    ~DebugBucket();

    DebugRenderable& getRenderable() const;

    const bool isRenderable;
    const bool isComplete;
    const optional<Timestamp> modified;
    const optional<Timestamp> expires;
    const MapDebugOptions debugMode;

private:
    std::unique_ptr<DebugRenderable> renderable;
};

} // namespace mbgl
