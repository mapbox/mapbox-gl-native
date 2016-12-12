
#include <test.hpp>

#include <protozero/types.hpp>

TEST_CASE("default constructed data_view") {
    protozero::data_view view;
    REQUIRE(view.data() == nullptr);
    REQUIRE(view.size() == 0);
}

TEST_CASE("data_view from C string") {
    protozero::data_view view{"foobar"};
    REQUIRE(view.data());
    REQUIRE(view.size() == 6);
}

TEST_CASE("data_view from std::string") {
    std::string str{"foobar"};
    protozero::data_view view{str};
    REQUIRE(view.data());
    REQUIRE(view.size() == 6);
}

TEST_CASE("data_view from ptr, size") {
    std::string str{"foobar"};
    protozero::data_view view{str.data(), str.size()};
    REQUIRE(view.data());
    REQUIRE(view.size() == 6);
}

TEST_CASE("convert data_view to std::string") {
    protozero::data_view view{"foobar"};

    std::string s = std::string(view);
    REQUIRE(s == "foobar");
    REQUIRE(std::string(view) == "foobar");
    REQUIRE(view.to_string() == "foobar");
}

TEST_CASE("converting default constructed data_view to string fails") {
    protozero::data_view view;
    REQUIRE_THROWS_AS({
        view.to_string();
    }, assert_error);
}

TEST_CASE("swapping data_view") {
    protozero::data_view view1{"foo"};
    protozero::data_view view2{"bar"};

    REQUIRE(view1.to_string() == "foo");
    REQUIRE(view2.to_string() == "bar");

    using std::swap;
    swap(view1, view2);

    REQUIRE(view2.to_string() == "foo");
    REQUIRE(view1.to_string() == "bar");
}

TEST_CASE("comparing data_views") {
    protozero::data_view v1{"foo"};
    protozero::data_view v2{"bar"};
    protozero::data_view v3{"foox"};
    protozero::data_view v4{"foo"};

    REQUIRE_FALSE(v1 == v2);
    REQUIRE_FALSE(v1 == v3);
    REQUIRE(v1 == v4);
    REQUIRE_FALSE(v2 == v3);
    REQUIRE_FALSE(v2 == v4);
    REQUIRE_FALSE(v3 == v4);

    REQUIRE(v1 != v2);
    REQUIRE(v1 != v3);
    REQUIRE_FALSE(v1 != v4);
    REQUIRE(v2 != v3);
    REQUIRE(v2 != v4);
    REQUIRE(v3 != v4);
}


