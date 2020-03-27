#pragma once

#include <mbgl/style/image.hpp>
#include <mbgl/util/immutable.hpp>

#include <exception>
#include <vector>

namespace mbgl {

namespace style {
class Image;
} // namespace style

class SpriteLoaderObserver {
public:
    virtual ~SpriteLoaderObserver() = default;

    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    virtual void onSpriteLoaded(std::vector<Immutable<style::Image::Impl>>) {}

    virtual void onSpriteError(std::exception_ptr) {}
};

} // namespace mbgl
