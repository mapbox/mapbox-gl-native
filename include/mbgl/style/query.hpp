#pragma once

#include <mbgl/util/optional.hpp>
#include <mbgl/style/filter.hpp>

#include <string>
#include <vector>

namespace mbgl {
namespace style {

/**
 * Options for query source features
 */
class SourceQueryOptions {
public:
    // Required for VectorSource, ignored for GeoJSONSource
    optional<std::vector<std::string>> sourceLayers;

    optional<style::Filter> filter;
};

} // namespace style
} // namespace mbgl
