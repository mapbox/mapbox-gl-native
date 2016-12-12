
#include <cstdint>

#include <test.hpp>

#include <protozero/byteswap.hpp>

namespace {

    int32_t check_swap_4(const int32_t data) {
        int32_t swapped;
        int32_t back;

        protozero::byteswap<4>(reinterpret_cast<const char*>(&data), reinterpret_cast<char*>(&swapped));
        protozero::byteswap<4>(reinterpret_cast<const char*>(&swapped), reinterpret_cast<char*>(&back));

        return back;
    }

    int64_t check_swap_8(const int64_t data) {
        int64_t swapped;
        int64_t back;

        protozero::byteswap<8>(reinterpret_cast<const char*>(&data), reinterpret_cast<char*>(&swapped));
        protozero::byteswap<8>(reinterpret_cast<const char*>(&swapped), reinterpret_cast<char*>(&back));

        return back;
    }

}

TEST_CASE("byte swapping") {
    REQUIRE(0 == check_swap_4(0));
    REQUIRE(1 == check_swap_4(1));
    REQUIRE(-1 == check_swap_4(-1));
    REQUIRE(395503 == check_swap_4(395503));
    REQUIRE(-804022 == check_swap_4(-804022));

    REQUIRE(0 == check_swap_8(0));
    REQUIRE(1 == check_swap_8(1));
    REQUIRE(-1 == check_swap_8(-1));
    REQUIRE(395503 == check_swap_8(395503));
    REQUIRE(-804022 == check_swap_8(-804022));
    REQUIRE(3280329805 == check_swap_8(3280329805));
    REQUIRE(-2489204041 == check_swap_8(-2489204041));
}

