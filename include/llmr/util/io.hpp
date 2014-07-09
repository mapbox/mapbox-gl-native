#ifndef LLMR_UTIL_IO
#define LLMR_UTIL_IO

#include <string>

namespace llmr {
namespace util {

void write_file(const std::string &filename, const std::string &data);
std::string read_file(const std::string &filename);

}
}

#endif
