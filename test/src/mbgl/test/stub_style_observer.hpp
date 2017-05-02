#pragma once

#include <mbgl/style/observer.hpp>

using namespace mbgl;
using namespace mbgl::style;

/**
 * An implementation of style::Observer that forwards all methods to dynamically-settable lambdas.
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

    void onSourceChanged(Source& source) override {
        if (sourceChanged) sourceChanged(source);
    }

    void onSourceError(Source& source, std::exception_ptr error) override {
        if (sourceError) sourceError(source, error);
    }

    void onSourceDescriptionChanged(Source& source) override {
        if (sourceDescriptionChanged) sourceDescriptionChanged(source);
    }

    void onResourceError(std::exception_ptr error) override {
        if (resourceError) resourceError(error);
    };

    std::function<void (const FontStack&, const GlyphRange&)> glyphsLoaded;
    std::function<void (const FontStack&, const GlyphRange&, std::exception_ptr)> glyphsError;
    std::function<void ()> spriteLoaded;
    std::function<void (std::exception_ptr)> spriteError;
    std::function<void (Source&)> sourceLoaded;
    std::function<void (Source&)> sourceChanged;
    std::function<void (Source&, std::exception_ptr)> sourceError;
    std::function<void (Source&)> sourceDescriptionChanged;
    std::function<void (std::exception_ptr)> resourceError;
};
