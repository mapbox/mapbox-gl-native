#include <mbgl/test/util.hpp>

#include <mbgl/util/pass_types.hpp>

TEST(Pass, NoCopy) {
    static int copyCount = 0;
    struct A {
        A() = default;
        A(A&&) = default;
        A(const A&) { ++copyCount; }
        static mbgl::Pass<A> create() { return A(); }
    };
    A a = A::create();
    (void)a;
    EXPECT_EQ(0, copyCount);
}