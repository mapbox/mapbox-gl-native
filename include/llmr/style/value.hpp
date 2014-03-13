#ifndef LLMR_STYLE_VALUE
#define LLMR_STYLE_VALUE

#include <llmr/util/variant.hpp>
#include <llmr/util/pbf.hpp>

namespace llmr {

typedef ::util::variant<bool, int64_t, uint64_t, double, std::string> Value;

Value parseValue(pbf data);

}

#endif
