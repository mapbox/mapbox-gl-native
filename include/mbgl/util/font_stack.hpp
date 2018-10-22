#pragma once

#include <mbgl/util/immutable.hpp>
#include <mbgl/style/layer.hpp>

#include <string>
#include <vector>
#include <set>

namespace mbgl {

// An array of font names
using FontStack = std::vector<std::string>;
using FontStackHash = std::size_t;

std::string fontStackToString(const FontStack&);

struct FontStackHasher {
    FontStackHash operator()(const FontStack&) const;
};

// Statically evaluate layer properties to determine what font stacks are used.
std::set<FontStack> fontStacks(const std::vector<Immutable<style::Layer::Impl>>&);

} // namespace mbgl
