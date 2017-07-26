#pragma once

#include <cstdint>

namespace mbgl {

enum class ResourceStatus : uint8_t {
    Success = 1,
    NotFoundError = 2,
    ServerError = 3,
    ConnectionError = 4,
    RateLimitError = 5,
    OtherError = 6,
};

} // namespace mbgl
