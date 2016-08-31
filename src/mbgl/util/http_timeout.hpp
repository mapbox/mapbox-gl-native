#pragma once

#include <mbgl/storage/response.hpp>
#include <mbgl/util/optional.hpp>
#include <mbgl/util/chrono.hpp>

namespace mbgl {
namespace http {

Duration errorRetryTimeout(Response::Error::Reason failedRequestReason, uint32_t failedRequests, optional<Timestamp> retryAfter = {});

Duration expirationTimeout(optional<Timestamp> expires, uint32_t expiredRequests);

} // namespace http
} // namespace mbgl
