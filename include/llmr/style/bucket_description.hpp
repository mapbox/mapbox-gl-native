#ifndef LLMR_STYLE_BUCKET_DESCRIPTION
#define LLMR_STYLE_BUCKET_DESCRIPTION

#include <string>
#include <vector>
#include <cmath>

#include <llmr/util/vec.hpp>
#include <llmr/map/filter_expression.hpp>
#include <llmr/style/value.hpp>
#include <llmr/style/style_bucket.hpp>

namespace llmr {

// Deprecated
class BucketDescription {
public:
    BucketType feature_type = BucketType::None;
    BucketType type = BucketType::None;

    // Specify what data to pull into this bucket
    std::string source_name;
    std::string source_layer;

    PropertyFilterExpression filter = std::true_type();

    // Specifies how the geometry for this bucket should be created
    StyleBucketRender render;
};

std::ostream& operator<<(std::ostream&, const BucketDescription& bucket);
std::ostream& operator<<(std::ostream&, BucketType type);

}

#endif
