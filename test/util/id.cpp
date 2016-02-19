#include "../fixtures/util.hpp"

#include <mbgl/util/id.hpp>

#include <set>
#include <sstream>
#include <unordered_set>

using namespace mbgl;

class Foo;

TEST(ID, Creation) {
    const util::ID<Foo> foo("foo");
    const util::ID<Foo> bar = { "bar" };
    const util::ID<Foo> baz;
    const util::ID<Foo> boo = { "" };

    EXPECT_TRUE(foo.valid());
    EXPECT_TRUE(bar.valid());
    EXPECT_FALSE(baz.valid());
    EXPECT_FALSE(boo.valid());
}

TEST(ID, Assignment) {
    const util::ID<Foo> foo("foo");
    util::ID<Foo> bar("bar");
    util::ID<Foo> baz;
    bar = "test";
    bar = foo;
    baz = "baz";

    EXPECT_TRUE(foo.valid());
    EXPECT_TRUE(bar.valid());
    EXPECT_TRUE(baz.valid());
}

TEST(ID, Comparison) {
    const util::ID<Foo> foo("foo");
    const util::ID<Foo> baz;

    EXPECT_EQ(foo, util::ID<Foo>("foo"));
    EXPECT_NE(foo, util::ID<Foo>("bar"));

    EXPECT_FALSE(baz == foo);
    EXPECT_FALSE(foo == baz);
    EXPECT_FALSE(baz == baz);

    EXPECT_TRUE(baz != foo);
    EXPECT_TRUE(foo != baz);
    EXPECT_TRUE(baz != baz);

    EXPECT_EQ("foo", foo.str());
    EXPECT_STREQ("foo", foo.c_str());
    EXPECT_EQ("", baz.str());
}

TEST(ID, Sorting) {
    std::set<util::ID<Foo>> set;
    set.emplace("foo");
    set.emplace("bar");

    const util::ID<Foo> a { "previously-unused-1" };
    const util::ID<Foo> b { "previously-unused-2" };
    EXPECT_TRUE(a < b);
    EXPECT_FALSE(a < a);
    EXPECT_FALSE(b < a);
}

TEST(ID, Hashing) {
    std::unordered_set<util::ID<Foo>> set;
    set.emplace("foo");
    set.emplace("bar");
}

TEST(ID, Stringstream) {
    util::ID<Foo> foo("foo");

    std::stringstream ss;
    ss << "Source[" << foo << "]";
    EXPECT_EQ("Source[foo]", ss.str());
}
