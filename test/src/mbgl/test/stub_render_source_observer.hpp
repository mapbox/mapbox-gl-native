#pragma once

#include <mbgl/renderer/render_source_observer.hpp>

using namespace mbgl;

/**
 * An implementation of style::Observer that forwards all methods to dynamically-settable lambdas.
 */
class StubRenderSourceObserver : public RenderSourceObserver {
public:
    void onTileChanged(RenderSource& source, const OverscaledTileID& tileID) override {
        if (tileChanged) tileChanged(source, tileID);
    };

    void
    onTileError(RenderSource& source, const OverscaledTileID& tileID, std::exception_ptr error) override {
        if (tileError) tileError(source, tileID, error);
    }

    std::function<void (RenderSource&, const OverscaledTileID&)> tileChanged;
    std::function<void (RenderSource&, const OverscaledTileID&, std::exception_ptr)> tileError;
};
