#ifndef MBGL_UTIL_INTERACTIVE_FEATURES
#define MBGL_UTIL_INTERACTIVE_FEATURES

#include <unordered_map>
#include <string>
#include <vector>
#include <tuple>

namespace mbgl {

// key, value
typedef std::unordered_map<std::string, std::string> FeatureProperties;

// layer name, source name, feature properties
typedef std::vector<std::tuple<std::string, std::string, FeatureProperties>> FeatureResults;

}

#endif
