#include <mbgl/util/id.hpp>

#include <atomic>

namespace mbgl {
namespace util {

uint64_t nextID() {
    static std::atomic<uint64_t> GUID { 0 };
    return GUID++;
}

} // namespace util
} // namespace mbgl
