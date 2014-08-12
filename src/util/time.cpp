#include <mbgl/util/time.hpp>
#include <mbgl/util/uv_detail.hpp>

namespace mbgl {
namespace util {

timestamp now() {
    return uv_hrtime();
}

}
}
