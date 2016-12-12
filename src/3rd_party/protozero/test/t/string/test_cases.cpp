
#include <test.hpp>

TEST_CASE("read string field using get_string") {

    SECTION("empty") {
        const std::string buffer = load_data("string/data-empty");

        protozero::pbf_reader item(buffer);

        REQUIRE(item.next());
        REQUIRE(item.get_string() == "");
        REQUIRE(!item.next());
    }

    SECTION("one") {
        const std::string buffer = load_data("string/data-one");

        protozero::pbf_reader item(buffer);

        REQUIRE(item.next());
        REQUIRE(item.get_string() == "x");
        REQUIRE(!item.next());
    }

    SECTION("string") {
        const std::string buffer = load_data("string/data-string");

        protozero::pbf_reader item(buffer);

        REQUIRE(item.next());
        REQUIRE(item.get_string() == "foobar");
        REQUIRE(!item.next());
    }

    SECTION("end_of_buffer") {
        const std::string buffer = load_data("string/data-string");

        for (std::string::size_type i = 1; i < buffer.size(); ++i) {
            protozero::pbf_reader item(buffer.data(), i);
            REQUIRE(item.next());
            REQUIRE_THROWS_AS(item.get_string(), protozero::end_of_buffer_exception);
        }
    }

}

TEST_CASE("read string field using get_view") {

    SECTION("empty") {
        const std::string buffer = load_data("string/data-empty");

        protozero::pbf_reader item(buffer);

        REQUIRE(item.next());
        auto v = item.get_view();
        REQUIRE(v.size() == 0);
        REQUIRE(!item.next());
    }

    SECTION("one") {
        const std::string buffer = load_data("string/data-one");

        protozero::pbf_reader item(buffer);

        REQUIRE(item.next());
        auto v = item.get_view();
        REQUIRE(*v.data() == 'x');
        REQUIRE(v.size() == 1);
        REQUIRE(!item.next());
    }

    SECTION("string") {
        const std::string buffer = load_data("string/data-string");

        protozero::pbf_reader item(buffer);

        REQUIRE(item.next());
        REQUIRE(std::string(item.get_view()) == "foobar");
        REQUIRE(!item.next());
    }

    SECTION("end_of_buffer") {
        const std::string buffer = load_data("string/data-string");

        for (std::string::size_type i = 1; i < buffer.size(); ++i) {
            protozero::pbf_reader item(buffer.data(), i);
            REQUIRE(item.next());
            REQUIRE_THROWS_AS(item.get_view(), protozero::end_of_buffer_exception);
        }
    }

}

TEST_CASE("write string field") {

    std::string buffer_test;
    protozero::pbf_writer pbf_test(buffer_test);

    SECTION("empty") {
        pbf_test.add_string(1, "");
        REQUIRE(buffer_test == load_data("string/data-empty"));
    }

    SECTION("one") {
        pbf_test.add_string(1, "x");
        REQUIRE(buffer_test == load_data("string/data-one"));
    }

    SECTION("string") {
        pbf_test.add_string(1, "foobar");
        REQUIRE(buffer_test == load_data("string/data-string"));
    }

}

