#include <mbgl/test/util.hpp>

#include <stdexcept>

TEST(Compiler, ExceptionQuirk) {
    try {
        throw std::invalid_argument("foo");
    } catch (const std::exception&) {
        SUCCEED();
        return;
    }

    FAIL() << "Should never be called";
}
