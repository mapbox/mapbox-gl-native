#ifndef LLMR_STYLE_BUCKET_DESCRIPTION
#define LLMR_STYLE_BUCKET_DESCRIPTION

#include <string>
#include <vector>

#include "value.hpp"

namespace llmr {

enum class BucketType {
    None = 0,
    Fill = 1,
    Line = 2,
    Point = 3
};

enum class CapType {
    None = 0,
    Round = 1,
};

enum class JoinType {
    None = 0,
    Butt = 1,
    Bevel = 2
};


class BucketDescription {
public:
    BucketType type = BucketType::None;

    // Specify what data to pull into this bucket
    std::string source_name;
    std::string source_layer;
    std::string source_field;
    std::vector<Value> source_value;

    // Specifies how the geometry for this bucket should be created
    CapType cap = CapType::None;
    JoinType join = JoinType::None;
    std::string font;
    float font_size = 0.0f;
};

std::ostream& operator<<(std::ostream&, const BucketDescription& bucket);

}

#endif
