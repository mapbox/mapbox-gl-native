#include <llmr/style/types.hpp>

#include <iostream>

namespace llmr {

BucketType parseBucketType(const std::string &type) {
    if (type == "fill") return BucketType::Fill;
    if (type == "line") return BucketType::Line;
    if (type == "icon") return BucketType::Icon;
    if (type == "text") return BucketType::Text;
    if (type == "raster") return BucketType::Raster;
    return BucketType::None;
}

const char *stringifyBucketType(BucketType type) {
    switch (type) {
        case BucketType::Fill: return "fill";
        case BucketType::Line: return "line";
        case BucketType::Icon: return "icon";
        case BucketType::Text: return "text";
        case BucketType::Raster: return "raster";
        default: return "none";
    }
}

std::ostream& operator<<(std::ostream& os, BucketType type) {
    return os << stringifyBucketType(type);
}

}
