#include <llmr/util/time.hpp>

#include <uv.h>

namespace llmr {
namespace util {

double time() {
    return (double)uv_hrtime() / 1e9;
}

}
}