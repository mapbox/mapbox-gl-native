#ifndef MBGL_UTIL_MERGELINES
#define MBGL_UTIL_MERGELINES

#include <map>
#include <string>
#include <vector>
#include <mbgl/renderer/symbol_bucket.hpp>

namespace mbgl {
namespace util {

unsigned int mergeFromRight(std::vector<SymbolFeature> &features,
                            std::map<std::string, unsigned int> &rightIndex,
                            std::map<std::string, unsigned int>::iterator left,
                            std::string &rightKey,
                            std::vector<std::vector<Coordinate>> &geom);

unsigned int mergeFromLeft(std::vector<SymbolFeature> &features,
                           std::map<std::string, unsigned int> &leftIndex,
                           std::string &leftKey,
                           std::map<std::string, unsigned int>::iterator right,
                           std::vector<std::vector<Coordinate>> &geom);

void mergeLines(std::vector<SymbolFeature> &features);

} // end namespace util
} // end namespace mbgl

#endif
