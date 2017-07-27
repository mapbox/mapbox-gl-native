#pragma once

#include <mbgl/storage/resource_kind.hpp>
#include <mbgl/storage/resource_status.hpp>

#include <stdexcept>

namespace mbgl {
namespace util {

struct ResourceError : public std::runtime_error {
    explicit ResourceError(const char* what_arg,
                           const ResourceKind kind_,
                           const ResourceStatus status_,
                           const std::string& url_)
        : std::runtime_error(what_arg), kind(kind_), status(status_), url(url_) {
    }
    explicit ResourceError(const std::string& what_arg,
                           const ResourceKind kind_,
                           const ResourceStatus status_,
                           const std::string& url_)
        : std::runtime_error(what_arg), kind(kind_), status(status_), url(url_) {
    }

    virtual ~ResourceError() = default;

    const ResourceKind kind;
    const ResourceStatus status;
    const std::string url;
};

} // namespace util
} // namespace mbgl
