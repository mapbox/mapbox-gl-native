
#include <test.hpp>

TEST_CASE("default constructed pbf_reader is okay") {
    protozero::pbf_reader item;

    REQUIRE(item.length() == 0);
    REQUIRE(!item); // test operator bool()
    REQUIRE(!item.next());
}

TEST_CASE("empty buffer in pbf_reader is okay") {
    const std::string buffer;
    protozero::pbf_reader item{buffer};

    REQUIRE(item.length() == 0);
    REQUIRE(!item); // test operator bool()
    REQUIRE(!item.next());
}

TEST_CASE("check every possible value for single byte in buffer") {
    char buffer;
    for (int i = 0; i <= 255; ++i) {
        buffer = static_cast<char>(i);
        protozero::pbf_reader item(&buffer, 1);

        REQUIRE(item.length() == 1);
        REQUIRE(!!item); // test operator bool()
        REQUIRE_THROWS({
            item.next();
            item.skip();
        });
    }
}

TEST_CASE("next() should throw when illegal wire type is encountered") {
    char buffer = 1 << 3 | 7;

    protozero::pbf_reader item{&buffer, 1};
    REQUIRE_THROWS_AS(item.next(), protozero::unknown_pbf_wire_type_exception);
}

