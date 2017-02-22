#pragma once

#include <mbgl/util/optional.hpp>
#include <mbgl/style/filter.hpp>

namespace mbgl {

/**
 * Options for Map queries.
 */
class QueryOptions {
public:
    /** layerIDs to include in the query */
    optional<std::vector<std::string>> layerIDs;
    
    optional<style::Filter> filter;
};

}
