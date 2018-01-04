#pragma once

#include <mbgl/style/image_impl.hpp>
#include <mbgl/style/source_impl.hpp>
#include <mbgl/style/layer_impl.hpp>
#include <mbgl/util/immutable.hpp>
#include <mbgl/util/variant.hpp>

#include <unordered_map>

namespace mbgl {

template <class T>
class StyleChange {
public:
    T before;
    T after;
};

template <class T>
class StyleDifference {
public:
    std::unordered_map<std::string, T> added;
    std::unordered_map<std::string, T> removed;
    std::unordered_map<std::string, StyleChange<T>> changed;
};

using ImmutableImage = Immutable<style::Image::Impl>;
using ImageDifference = StyleDifference<ImmutableImage>;

ImageDifference diffImages(const Immutable<std::vector<ImmutableImage>>&,
                           const Immutable<std::vector<ImmutableImage>>&);

using ImmutableSource = Immutable<style::Source::Impl>;
using SourceDifference = StyleDifference<ImmutableSource>;

SourceDifference diffSources(const Immutable<std::vector<ImmutableSource>>&,
                             const Immutable<std::vector<ImmutableSource>>&);

using ImmutableLayer = Immutable<style::Layer::Impl>;
using LayerDifference = StyleDifference<ImmutableLayer>;

LayerDifference diffLayers(const Immutable<std::vector<ImmutableLayer>>&,
                           const Immutable<std::vector<ImmutableLayer>>&);

bool hasLayoutDifference(const LayerDifference&, const std::string& layerID);

} // namespace mbgl
