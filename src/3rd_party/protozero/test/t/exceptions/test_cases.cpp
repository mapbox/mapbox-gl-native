
#include <test.hpp>

TEST_CASE("exceptions messages for pbf exception") {
    protozero::exception e;
    REQUIRE(std::string{e.what()} == std::string{"pbf exception"});
}

TEST_CASE("exceptions messages for varint too long") {
    protozero::varint_too_long_exception e;
    REQUIRE(std::string{e.what()} == std::string{"varint too long exception"});
}

TEST_CASE("exceptions messages for unknown pbf field type") {
    protozero::unknown_pbf_wire_type_exception e;
    REQUIRE(std::string{e.what()} == std::string{"unknown pbf field type exception"});
}

TEST_CASE("exceptions messages for end of buffer") {
    protozero::end_of_buffer_exception e;
    REQUIRE(std::string{e.what()} == std::string{"end of buffer exception"});
}

