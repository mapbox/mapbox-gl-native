#include <mbgl/test/util.hpp>

#include <mbgl/util/unique_any.hpp>

using namespace mbgl::util;

class TestType {
public:
    TestType() : i1(0), i2(1) {
        str[0] = 'a';
    }
    
    TestType(unique_any& p) : TestType() {
        p = std::unique_ptr<TestType>(this);
    }

    //Detect moves
    TestType(TestType&& t): i1(t.i1+1), i2(t.i2+2) {
        str[0] = t.str[0]+1;
    }

    int i1;
    int i2;
    char str[256];
};

bool  IsStackAllocated (const unique_any& a, const void* obj1) {
    uintptr_t a_ptr = (uintptr_t)(&a);
    uintptr_t obj   = (uintptr_t)(obj1);
    return (obj >= a_ptr && obj < a_ptr + sizeof(unique_any));
};

TEST(UniqueAny, Empty) {
    EXPECT_FALSE(unique_any().has_value());
    EXPECT_TRUE(unique_any().type() == typeid(void));
    EXPECT_THROW(any_cast<int>(unique_any()), bad_any_cast);
}

TEST(UniqueAny, BasicTypes) {
    unique_any i = 3;
    EXPECT_TRUE(i.has_value());
    EXPECT_TRUE(i.type() == typeid(int));
    EXPECT_TRUE(IsStackAllocated(i, any_cast<int>(&i)));

    auto iValue = any_cast<int>(i);
    EXPECT_TRUE(iValue == 3);

    EXPECT_TRUE(unique_any(4).has_value());
    EXPECT_TRUE(unique_any(4).type() == typeid(int));

    unique_any f = 6.2f;
    EXPECT_TRUE(f.has_value());
    EXPECT_TRUE(f.type() == typeid(float));
    EXPECT_TRUE(IsStackAllocated(f, any_cast<float>(&f)));

    const float fValue = any_cast<const float>(f);
    EXPECT_TRUE(fValue == 6.2f);

    EXPECT_TRUE(unique_any(1.0f).has_value());
    EXPECT_TRUE(unique_any(1.0f).type() == typeid(float));

    unique_any c = 'z';
    EXPECT_TRUE(c.has_value());
    EXPECT_TRUE(c.type() == typeid(char));
    EXPECT_TRUE(IsStackAllocated(c, any_cast<char>(&c)));

    EXPECT_THROW(any_cast<float>(c), bad_any_cast);

    EXPECT_TRUE(unique_any('4').has_value());
    EXPECT_TRUE(unique_any('4').type() == typeid(char));
}

TEST(UniqueAny, BasicTypes_Move) {
    unique_any i = 3;
    EXPECT_TRUE(i.has_value());
    EXPECT_TRUE(i.type() == typeid(int));

    unique_any f = 6.2f;
    EXPECT_TRUE(f.has_value());
    EXPECT_TRUE(f.type() == typeid(float));

    f = std::move(i);
    EXPECT_FALSE(i.has_value());
    EXPECT_TRUE(i.type() == typeid(void));

    EXPECT_TRUE(f.has_value());
    EXPECT_TRUE(f.type() == typeid(int));

}

TEST(UniqueAny, LargeType) {
    TestType t1;
    unique_any u1 = unique_any(std::move(t1));
    EXPECT_TRUE(u1.has_value());
    EXPECT_TRUE(u1.type() == typeid(TestType));
    EXPECT_FALSE(IsStackAllocated(u1, any_cast<TestType>(&u1)));

    //TestType should be moved into owning unique_any
    EXPECT_EQ(any_cast<TestType>(&u1)->i1, 1);

    auto u2(std::move(u1));
    EXPECT_TRUE(u2.type() == typeid(TestType));
    EXPECT_TRUE(u1.type() == typeid(void));

    //TestType should not be moved when owning unique_any is moved;
    EXPECT_EQ(any_cast<TestType>(&u2)->i1, 1);

    //TestType should be moved out of owning unique_any
    // Note: two moves are involved in returning the moved value
    //  First out of the unique_any, and then in the return statement
    auto t2 = any_cast<TestType>(std::move(u2));
    EXPECT_EQ(t2.i1, 3);
    EXPECT_TRUE(u2.type() == typeid(void));
}

TEST(UniqueAny, Pointer) {
    auto t1 = new TestType();

    auto u1 = unique_any(std::move(t1));
    EXPECT_TRUE(u1.has_value());
    EXPECT_TRUE(u1.type() == typeid(TestType *));
    EXPECT_TRUE(IsStackAllocated(u1, any_cast<TestType *>(&u1)));

    //Only the pointer should be moved
    TestType * t2 = *any_cast<TestType *>(&u1);
    EXPECT_EQ(t2->i1, 0);

    unique_any u2(4);
    std::swap(u2, u1);

    EXPECT_TRUE(u1.has_value());
    EXPECT_TRUE(u1.type() == typeid(int));

    EXPECT_TRUE(u2.has_value());
    EXPECT_TRUE(u2.type() == typeid(TestType *));
    
    t2 = *any_cast<TestType *>(&u2);
    EXPECT_EQ(t2->i1, 0);
    delete t2;
}


TEST(UniqueAny, UniquePtr) {
    auto t1 = std::make_unique<TestType>();
    auto u1 = unique_any(std::move(t1));

    EXPECT_EQ(t1.get(), nullptr);
    EXPECT_TRUE(u1.has_value());
    EXPECT_TRUE(u1.type() == typeid(std::unique_ptr<TestType>));

    EXPECT_TRUE(IsStackAllocated(u1, any_cast<std::unique_ptr<TestType>>(&u1)));

    auto t2 = any_cast<std::unique_ptr<TestType> >(std::move(u1));
    EXPECT_FALSE(u1.has_value());
    
    unique_any u2;
    TestType * t3 = new TestType();
    u2 = std::unique_ptr<TestType>(t3);
    EXPECT_TRUE(u2.has_value());
    EXPECT_TRUE(any_cast<std::unique_ptr<TestType>>(&u2)->get() == t3);
}

TEST(UniqueAny, SharedPtr) {

    std::shared_ptr<int> shared(new int(3));
    std::weak_ptr<int> weak = shared;
    unique_any u1 = 0;

    EXPECT_THROW(any_cast<float>(u1), bad_any_cast);

    EXPECT_EQ(weak.use_count(), 1);
    unique_any u2 = shared;
    EXPECT_EQ(weak.use_count(), 2);

    EXPECT_EQ(any_cast<std::unique_ptr<int>>(&u1), nullptr);
    EXPECT_FALSE(IsStackAllocated(u1, any_cast<std::shared_ptr<TestType>>(&u1)));

    u1 = std::move(u2);
    EXPECT_EQ(weak.use_count(), 2);
    u2.swap(u1);
    EXPECT_EQ(weak.use_count(), 2);
    u2 = 0;
    EXPECT_EQ(weak.use_count(), 1);
    shared = nullptr;
    EXPECT_EQ(weak.use_count(), 0);
}
