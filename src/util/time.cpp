#include <mbgl/util/time.hpp>

#include <uv.h>

namespace mbgl {
namespace util {

timestamp now() {
    return uv_hrtime();
}

}
}
