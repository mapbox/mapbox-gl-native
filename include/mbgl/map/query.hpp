#pragma once

#include <mbgl/util/optional.hpp>
#include <mbgl/style/filter.hpp>

#include <functional>

namespace mbgl {

using namespace style;
    
    /** Options for Map queries.
     
     */
    struct QueryOptions {
    public:
        /** layerIDs to include in the query */
        optional<std::vector<std::string>> layerIDs;
        
        optional<Filter> filter;
        
        QueryOptions() {}
        
        QueryOptions(optional<std::vector<std::string>> l) : layerIDs (l) {}
        
        QueryOptions(optional<std::vector<std::string>> l, optional<Filter> f) : layerIDs (l),
            filter(f) {
        }
    };
    
}
