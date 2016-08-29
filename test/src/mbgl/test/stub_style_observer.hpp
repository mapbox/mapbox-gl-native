#pragma once

#include <mbgl/style/observer.hpp>

using namespace mbgl;
using namespace mbgl::style;

/**
 * An implementation of style::Observer that forwards all methods to dynamically-settable lambas.
 */
class StubStyleObserver : public style::Observer {
public:
    void onGlyphsLoaded(const FontStack& fontStack, const GlyphRange& glyphRange) override {
        if (glyphsLoaded) glyphsLoaded(fontStack, glyphRange);
    }

    void onGlyphsError(const FontStack& fontStack, const GlyphRange& glyphRange, std::exception_ptr error) override {
        if (glyphsError) glyphsError(fontStack, glyphRange, error);
    }

    void onSpriteLoaded() override {
        if (spriteLoaded) spriteLoaded();
    }

    void onSpriteError(std::exception_ptr error) override {
        if (spriteError) spriteError(error);
    }

    void onSourceLoaded(Source& source) override {
        if (sourceLoaded) sourceLoaded(source);
    }

    void onSourceError(Source& source, std::exception_ptr error) override {
        if (sourceError) sourceError(source, error);
    }

    void onTileLoaded(Source& source, const OverscaledTileID& tileID, bool isNewTile) override {
        if (tileLoaded) tileLoaded(source, tileID, isNewTile);
    }

    void
    onTileError(Source& source, const OverscaledTileID& tileID, std::exception_ptr error) override {
        if (tileError) tileError(source, tileID, error);
    }

    void onTileUpdated(Source& source, const OverscaledTileID& tileID) override {
        if (tileUpdated) tileUpdated(source, tileID);
    };

    void onResourceError(std::exception_ptr error) override {
        if (resourceError) resourceError(error);
    };

    std::function<void (const FontStack&, const GlyphRange&)> glyphsLoaded;
    std::function<void (const FontStack&, const GlyphRange&, std::exception_ptr)> glyphsError;
    std::function<void ()> spriteLoaded;
    std::function<void (std::exception_ptr)> spriteError;
    std::function<void (Source&)> sourceLoaded;
    std::function<void (Source&, std::exception_ptr)> sourceError;
    std::function<void (Source&, const OverscaledTileID&, bool isNewTile)> tileLoaded;
    std::function<void (Source&, const OverscaledTileID&, std::exception_ptr)> tileError;
    std::function<void (Source&, const OverscaledTileID&)> tileUpdated;
    std::function<void (std::exception_ptr)> resourceError;
};
