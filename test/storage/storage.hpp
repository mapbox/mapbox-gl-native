#ifndef MBGL_TEST_STORAGE_STORAGE
#define MBGL_TEST_STORAGE_STORAGE

#include "../fixtures/util.hpp"
#include <mbgl/storage/response.hpp>
#include <uv.h>
#include <iostream>

class Storage : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();

protected:
    static pid_t pid;
};

namespace mbgl {

inline std::ostream& operator<<(std::ostream& os, Response::Error::Reason r) {
    // Special case
    if (uint8_t(r) == 1) return os << "Response::Error::Reason::Success";
    switch (r) {
        case Response::Error::Reason::NotFound: return os << "Response::Error::Reason::NotFound";
        case Response::Error::Reason::Server: return os << "Response::Error::Reason::Server";
        case Response::Error::Reason::Connection: return os << "Response::Error::Reason::Connection";
        case Response::Error::Reason::Canceled: return os << "Response::Error::Reason::Canceled";
        case Response::Error::Reason::Other: return os << "Response::Error::Reason::Other";
        default: return os << "<Unknown>";
    }
}

} // namespace mbgl

#endif
