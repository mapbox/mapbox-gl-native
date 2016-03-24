#pragma once

#include <mbgl/util/noncopyable.hpp>

namespace mbgl {

class AsyncRequest : private util::noncopyable {
public:
    virtual ~AsyncRequest() = default;
};

} // namespace mbgl
