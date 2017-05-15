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

    virtual void onSpriteLoaded(std::vector<std::unique_ptr<style::Image>>&&) {}
    virtual void onSpriteError(std::exception_ptr) {}
};

} // namespace mbgl
