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
    Butt = 2,
    Square = 3
};

enum class JoinType {
    None = 0,
    Miter = 1,
    Bevel = 2,
    Round = 3
};


inline BucketType bucketType(const std::string& type) {
    if (type == "fill") return BucketType::Fill;
    else if (type == "line") return BucketType::Line;
    else if (type == "point") return BucketType::Point;
    else return BucketType::None;
}

inline CapType capType(const std::string& cap) {
    if (cap == "round") return CapType::Round;
    else if (cap == "butt") return CapType::Butt;
    else if (cap == "square") return CapType::Square;
    else return CapType::None;
}


inline JoinType joinType(const std::string& join) {
    if (join == "miter") return JoinType::Miter;
    else if (join == "bevel") return JoinType::Bevel;
    else if (join == "round") return JoinType::Round;
    else return JoinType::None;
}

class BucketGeometryDescription {
public:
    CapType cap = CapType::None;
    JoinType join = JoinType::None;
    std::string font;
    float font_size = 0.0f;
    float miter_limit = 2.0f;
    float round_limit = 1.0f;
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
    BucketGeometryDescription geometry;
};

std::ostream& operator<<(std::ostream&, const BucketDescription& bucket);

}

#endif
