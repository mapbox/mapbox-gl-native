#include <llmr/util/time.hpp>

#include <uv.h>

namespace llmr {
namespace util {

float time() {
    return (double)uv_hrtime() / 1e6;
}

}
}
