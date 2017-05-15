#pragma once

#include <mbgl/style/image.hpp>

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

namespace mbgl {

using Images = std::unordered_map<std::string, std::unique_ptr<style::Image>>;

void addSpriteImage(Images&,
                    std::unique_ptr<style::Image>,
                    std::function<void (style::Image&)> onAdded = [] (style::Image&){});

void removeSpriteImage(Images&,
                       const std::string&,
                       std::function<void ()> onRemoved = [] (){});



} // namespace mbgl
