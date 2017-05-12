#pragma once

#include <mbgl/style/observer.hpp>

using namespace mbgl;
using namespace mbgl::style;

/**
 * An implementation of style::Observer that forwards all methods to dynamically-settable lambdas.
 */
class StubStyleObserver : public style::Observer {
public:
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

    std::function<void (Source&)> sourceLoaded;
    std::function<void (Source&)> sourceChanged;
    std::function<void (Source&, std::exception_ptr)> sourceError;
    std::function<void (Source&)> sourceDescriptionChanged;
    std::function<void (std::exception_ptr)> resourceError;
};
