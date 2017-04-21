#pragma once

#include <exception>

namespace mbgl {

class RenderSource;
class OverscaledTileID;

class RenderSourceObserver {
public:
    virtual ~RenderSourceObserver() = default;

    virtual void onTileChanged(RenderSource&, const OverscaledTileID&) {}
    virtual void onTileError(RenderSource&, const OverscaledTileID&, std::exception_ptr) {}
};

} // namespace mbgl
