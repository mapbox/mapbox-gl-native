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
    RenderedQueryOptions(optional<std::vector<std::string>> layerIDs_ = {},
                         optional<style::Filter> filter_ = {})
        : layerIDs(std::move(layerIDs_)),
          filter(std::move(filter_)) {}

    /** layerIDs to include in the query */
    optional<std::vector<std::string>> layerIDs;

    optional<style::Filter> filter;
};

/**
 * Options for query source features
 */
class SourceQueryOptions {
public:
    SourceQueryOptions(optional<std::vector<std::string>> sourceLayers_ = {},
                       optional<style::Filter> filter_ = {})
        : sourceLayers(std::move(sourceLayers_)),
          filter(std::move(filter_)) {}

    // Required for VectorSource, ignored for GeoJSONSource
    optional<std::vector<std::string>> sourceLayers;

    optional<style::Filter> filter;
};

} // namespace mbgl
