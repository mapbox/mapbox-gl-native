#ifndef LLMR_UTIL_IMAGE
#define LLMR_UTIL_IMAGE

#include <string>

namespace llmr {
namespace util {

std::string compress_png(int width, int height, void *rgba);

}
}

#endif
