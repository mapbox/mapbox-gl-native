#pragma once

#include <exception>

namespace mbgl {

class OverscaledTileID;

namespace style {

class Source;

class SourceObserver {
public:
    virtual ~SourceObserver() = default;

    virtual void onSourceLoaded(Source&) {}
    virtual void onSourceError(Source&, std::exception_ptr) {}

    virtual void onTileLoaded(Source&, const OverscaledTileID&, bool /* isNewTile */) {}
    virtual void onTileError(Source&, const OverscaledTileID&, std::exception_ptr) {}
    virtual void onTileUpdated(Source&, const OverscaledTileID&) {}
};

} // namespace style
} // namespace mbgl
