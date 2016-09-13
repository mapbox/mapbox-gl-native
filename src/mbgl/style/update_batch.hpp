#pragma once

#include <unordered_set>
#include <string>

namespace mbgl {
namespace style {

class UpdateBatch {
public:
    std::unordered_set<std::string> sourceIDs;
};

} // namespace style
} // namespace mbgl
