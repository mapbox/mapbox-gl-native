#pragma once

#include <exception>
#include <memory>
#include <unordered_map>
#include <string>

namespace mbgl {

namespace style {
class Image;
} // namespace style

class SpriteLoaderObserver {
public:
    using Images = std::unordered_map<std::string, std::unique_ptr<style::Image>>;

    virtual ~SpriteLoaderObserver() = default;

    virtual void onSpriteLoaded(Images&&) {}
    virtual void onSpriteError(std::exception_ptr) {}
};

} // namespace mbgl
