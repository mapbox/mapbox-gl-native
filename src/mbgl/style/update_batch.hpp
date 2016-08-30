#pragma once

#include <set>
#include <string>

namespace mbgl {
namespace style {

class UpdateBatch {
public:
    std::set<std::string> sourceIDs;
};

} // namespace style
} // namespace mbgl
