#include <mbgl/storage/response.hpp>
#include <mbgl/util/chrono.hpp>

namespace mbgl {

bool Response::isExpired() const {
    const int64_t now = std::chrono::duration_cast<std::chrono::seconds>(
                                    SystemClock::now().time_since_epoch()).count();
    return expires <= now;
}

} // namespace mbgl
