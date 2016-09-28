#pragma once

#include <mbgl/tile/tile_observer.hpp>

#include <exception>

namespace mbgl {

class OverscaledTileID;

namespace style {

class Source;

class SourceObserver {
public:
    virtual ~SourceObserver() = default;

    virtual void onSourceLoaded(Source&) {}
    virtual void onSourceAttributionChanged(Source&, const std::string&) {}
    virtual void onSourceError(Source&, std::exception_ptr) {}

    virtual void onTileChanged(Source&, const OverscaledTileID&) {}
    virtual void onTileError(Source&, const OverscaledTileID&, std::exception_ptr) {}
};

} // namespace style
} // namespace mbgl
