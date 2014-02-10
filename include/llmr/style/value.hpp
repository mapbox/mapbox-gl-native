#ifndef LLMR_STYLE_VALUE
#define LLMR_STYLE_VALUE

#include <boost/variant.hpp>
#include <llmr/util/pbf.hpp>

namespace llmr {

typedef boost::variant<bool, int64_t, uint64_t, double, std::string> Value;

Value parseValue(pbf data);

}

#endif
