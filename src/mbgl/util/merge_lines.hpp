#pragma once

#include <unordered_map>
#include <string>
#include <vector>
#include <mbgl/layout/symbol_layout.hpp>

namespace mbgl {
namespace util {

unsigned int mergeFromRight(std::vector<SymbolFeature> &features,
                            std::unordered_map<std::string, unsigned int> &rightIndex,
                            std::unordered_map<std::string, unsigned int>::iterator left,
                            std::string &rightKey,
                            GeometryCollection &geom);

unsigned int mergeFromLeft(std::vector<SymbolFeature> &features,
                           std::unordered_map<std::string, unsigned int> &leftIndex,
                           std::string &leftKey,
                           std::unordered_map<std::string, unsigned int>::iterator right,
                           GeometryCollection &geom);

void mergeLines(std::vector<SymbolFeature> &features);

} // end namespace util
} // end namespace mbgl
