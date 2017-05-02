#pragma once

#include <mbgl/util/optional.hpp>
#include <mbgl/style/filter.hpp>

#include <string>
#include <vector>

namespace mbgl {

/**
 * Options for query rendered features.
 */
class RenderedQueryOptions {
public:
    /** layerIDs to include in the query */
    optional<std::vector<std::string>> layerIDs;

    optional<style::Filter> filter;
};

/**
 * Options for query source features
 */
class SourceQueryOptions {
public:
    // Required for VectorSource, ignored for GeoJSONSource
    optional<std::vector<std::string>> sourceLayers;

    optional<style::Filter> filter;
};

}
