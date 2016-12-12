
#include <test.hpp>

TEST_CASE("max varint length") {
    REQUIRE(protozero::max_varint_length == 10);
}

TEST_CASE("varint") {

    std::string buffer;
    protozero::pbf_writer pw{buffer};

    SECTION("encode/decode int32") {
        pw.add_int32(1, 17);
        protozero::pbf_reader item{buffer};
        REQUIRE(item.next());

        SECTION("get") {
            REQUIRE(17 == item.get_int32());
        }

        SECTION("skip") {
            item.skip();
        }

        REQUIRE_FALSE(item.next());
    }

    SECTION("encode/decode uint32") {
        pw.add_uint32(1, 17U);
        protozero::pbf_reader item{buffer};
        REQUIRE(item.next());

        SECTION("get") {
            REQUIRE(17U == item.get_uint32());
        }

        SECTION("skip") {
            item.skip();
        }

        REQUIRE_FALSE(item.next());
    }

    SECTION("encode/decode uint64") {
        pw.add_uint64(1, (1ULL << 40));
        protozero::pbf_reader item{buffer};
        REQUIRE(item.next());

        SECTION("get") {
            REQUIRE((1ULL << 40) == item.get_uint64());
        }

        SECTION("skip") {
            item.skip();
        }

        REQUIRE_FALSE(item.next());
    }

    SECTION("short buffer while parsing varint") {
        pw.add_uint64(1, (1ULL << 40));
        buffer.resize(buffer.size() - 1); // "remove" last byte from buffer
        protozero::pbf_reader item{buffer};
        REQUIRE(item.next());

        SECTION("get") {
            REQUIRE_THROWS_AS(item.get_uint64(), protozero::end_of_buffer_exception);
        }

        SECTION("skip") {
            REQUIRE_THROWS_AS(item.skip(), protozero::end_of_buffer_exception);
        }
    }

    SECTION("data corruption in buffer while parsing varint)") {
        pw.add_uint64(1, (1ULL << 20));
        buffer[buffer.size() - 1] += 0x80; // pretend the varint goes on
        protozero::pbf_reader item{buffer};
        REQUIRE(item.next());

        SECTION("get") {
            REQUIRE_THROWS_AS(item.get_uint64(), protozero::end_of_buffer_exception);
        }

        SECTION("skip") {
            REQUIRE_THROWS_AS(item.skip(), protozero::end_of_buffer_exception);
        }
    }

    SECTION("data corruption in buffer while parsing varint (max length varint)") {
        pw.add_uint64(1, std::numeric_limits<uint64_t>::max());
        buffer[buffer.size() - 1] += 0x80; // pretend the varint goes on
        protozero::pbf_reader item{buffer};
        REQUIRE(item.next());

        SECTION("get") {
            REQUIRE_THROWS_AS(item.get_uint64(), protozero::varint_too_long_exception);
        }

        SECTION("skip") {
            REQUIRE_THROWS_AS(item.skip(), protozero::varint_too_long_exception);
        }
    }

}

TEST_CASE("lots of varints back and forth") {

    std::string buffer;

    for (uint32_t n = 0; n < 70000; ++n) {
        protozero::pbf_writer pw{buffer};
        pw.add_uint32(1, n);
        protozero::pbf_reader item{buffer};
        REQUIRE(item.next());
        REQUIRE(n == item.get_uint32());
        REQUIRE(!item.next());
        buffer.clear();
    }

    for (int32_t n = -70000; n < 70000; ++n) {
        protozero::pbf_writer pw{buffer};
        pw.add_int32(1, n);
        protozero::pbf_reader item{buffer};
        REQUIRE(item.next());
        REQUIRE(n == item.get_int32());
        REQUIRE(!item.next());
        buffer.clear();
    }

    for (int32_t n = -70000; n < 70000; ++n) {
        protozero::pbf_writer pw{buffer};
        pw.add_sint32(1, n);
        protozero::pbf_reader item{buffer};
        REQUIRE(item.next());
        REQUIRE(n == item.get_sint32());
        REQUIRE(!item.next());
        buffer.clear();
    }

}

