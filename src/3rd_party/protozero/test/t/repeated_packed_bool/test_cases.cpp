
#include <test.hpp>

TEST_CASE("read repeated packed bool field") {

    SECTION("empty") {
        const std::string buffer = load_data("repeated_packed_bool/data-empty");

        protozero::pbf_reader item(buffer);

        REQUIRE(!item.next());
    }

    SECTION("one") {
        const std::string buffer = load_data("repeated_packed_bool/data-one");

        protozero::pbf_reader item(buffer);

        REQUIRE(item.next());
        auto it_range = item.get_packed_bool();
        REQUIRE(!item.next());

        REQUIRE(it_range.begin() != it_range.end());
        REQUIRE(*it_range.begin());
        REQUIRE(std::next(it_range.begin()) == it_range.end());
    }

    SECTION("many") {
        const std::string buffer = load_data("repeated_packed_bool/data-many");

        protozero::pbf_reader item(buffer);

        REQUIRE(item.next());
        auto it_range = item.get_packed_bool();
        REQUIRE(!item.next());

        auto it = it_range.begin();
        REQUIRE(it != it_range.end());
        REQUIRE(*it++);
        REQUIRE(*it++);
        REQUIRE(! *it++);
        REQUIRE(*it++);
        REQUIRE(it == it_range.end());
    }

    SECTION("end_of_buffer") {
        const std::string buffer = load_data("repeated_packed_bool/data-many");

        for (std::string::size_type i = 1; i < buffer.size(); ++i) {
            protozero::pbf_reader item(buffer.data(), i);
            REQUIRE(item.next());
            REQUIRE_THROWS_AS(item.get_packed_bool(), protozero::end_of_buffer_exception);
        }
    }

}

TEST_CASE("write repeated packed bool field") {

    std::string buffer;
    protozero::pbf_writer pw(buffer);

    SECTION("empty") {
        bool data[] = { true };
        pw.add_packed_bool(1, std::begin(data), std::begin(data) /* !!!! */);

        REQUIRE(buffer == load_data("repeated_packed_bool/data-empty"));
    }

    SECTION("one") {
        bool data[] = { true };
        pw.add_packed_bool(1, std::begin(data), std::end(data));

        REQUIRE(buffer == load_data("repeated_packed_bool/data-one"));
    }

    SECTION("many") {
        bool data[] = { true, true, false, true };
        pw.add_packed_bool(1, std::begin(data), std::end(data));

        REQUIRE(buffer == load_data("repeated_packed_bool/data-many"));
    }

}

TEST_CASE("write repeated packed bool field using packed_field_bool") {

    std::string buffer;
    protozero::pbf_writer pw(buffer);

    SECTION("empty - should do rollback") {
        {
            protozero::packed_field_bool field{pw, 1};
        }

        REQUIRE(buffer == load_data("repeated_packed_bool/data-empty"));
    }

    SECTION("one") {
        {
            protozero::packed_field_bool field{pw, 1};
            field.add_element(true);
        }

        REQUIRE(buffer == load_data("repeated_packed_bool/data-one"));
    }

    SECTION("many") {
        {
            protozero::packed_field_bool field{pw, 1};
            field.add_element(true);
            field.add_element(true);
            field.add_element(false);
            field.add_element(true);
        }

        REQUIRE(buffer == load_data("repeated_packed_bool/data-many"));
    }

}

