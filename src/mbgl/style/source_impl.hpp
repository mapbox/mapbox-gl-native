#pragma once

#include <mbgl/style/source.hpp>
#include <mbgl/util/noncopyable.hpp>

#include <string>

namespace mbgl {

class RenderSource;

namespace style {

class SourceObserver;

class Source::Impl {
public:
    virtual ~Impl() = default;

    Impl& operator=(const Impl&) = delete;

    virtual optional<std::string> getAttribution() const = 0;
    void setPrefetchZoomDelta(optional<uint8_t> delta) noexcept;
    optional<uint8_t> getPrefetchZoomDelta() const noexcept;
    void setMinimumTileUpdateInterval(Duration interval) { minimumTileUpdateInterval = interval; }
    Duration getMinimumTileUpdateInterval() const { return minimumTileUpdateInterval; }
    void setMaxOverscaleFactorForParentTiles(optional<uint8_t> overscaleFactor) noexcept;
    optional<uint8_t> getMaxOverscaleFactorForParentTiles() const noexcept;

    const SourceType type;
    const std::string id;

protected:
    optional<uint8_t> prefetchZoomDelta;
    optional<uint8_t> maxOverscaleFactor;
    Duration minimumTileUpdateInterval{Duration::zero()};

    Impl(SourceType, std::string);
    Impl(const Impl&) = default;
};

} // namespace style
} // namespace mbgl
