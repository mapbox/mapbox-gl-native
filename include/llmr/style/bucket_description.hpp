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

std::ostream& operator<<(std::ostream&, BucketType type);

}

#endif
