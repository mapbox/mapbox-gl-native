#ifndef LLMR_STYLE_VALUE
#define LLMR_STYLE_VALUE

#include <boost/variant.hpp>
#include <llmr/util/pbf.hpp>

namespace llmr {

typedef boost::variant<std::string, double, int64_t, uint64_t, bool> Value;

Value parseValue(pbf data);

}

#endif
