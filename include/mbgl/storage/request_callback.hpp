#ifndef MBGL_STORAGE_REQUEST_CALLBACK
#define MBGL_STORAGE_REQUEST_CALLBACK

#include <mbgl/util/variant.hpp>

#include <functional>

namespace mbgl {

class Response;

using CompletedCallback = std::function<void(const Response &)>;
using AbortedCallback = std::function<void()>;

using Callback = mapbox::util::variant<
    CompletedCallback,
    AbortedCallback
>;

}

#endif
