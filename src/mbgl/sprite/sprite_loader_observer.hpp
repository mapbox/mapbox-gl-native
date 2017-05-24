#pragma once

#include <exception>
#include <memory>
#include <vector>

namespace mbgl {

namespace style {
class Image;
} // namespace style

class SpriteLoaderObserver {
public:
    virtual ~SpriteLoaderObserver() = default;

    using Images = std::vector<std::unique_ptr<style::Image>>;
    virtual void onSpriteLoaded(Images&&) {}
    virtual void onSpriteError(std::exception_ptr) {}
};

} // namespace mbgl
