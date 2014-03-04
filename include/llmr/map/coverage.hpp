#ifndef LLMR_MAP_COVERAGE
#define LLMR_MAP_COVERAGE

#include <llmr/util/vec.hpp>
#include <forward_list>

namespace llmr {

std::forward_list<vec3<int32_t>> covering_tiles(int32_t zoom, const box& points);

}

#endif
