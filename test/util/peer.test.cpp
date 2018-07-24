#include <mbgl/test/util.hpp>

#include <mbgl/util/peer.hpp>

using namespace mbgl::util;

class TestType {
public:
    TestType() : i1(0), i2(1) {
        str[0] = 'a';
    }
    
    //Detect moves
    TestType(TestType&& t): i1(t.i1+1), i2(t.i2+2) {
        str[0] = t.str[0]+1;
    }

    TestType(const TestType&) = delete;
    TestType& operator=(const TestType&) = delete;

    int i1;
    int i2;
    char str[256];
};

bool  IsStackAllocated (const peer& a, const void* obj1) {
    uintptr_t a_ptr = (uintptr_t)(&a);
    uintptr_t obj   = (uintptr_t)(obj1);
    return (obj >= a_ptr && obj < a_ptr + sizeof(peer));
};

TEST(Peer, Empty) {
    EXPECT_FALSE(peer().has_value());
}

TEST(Peer, BasicTypes) {
    peer i = 3;
    EXPECT_TRUE(i.has_value());
    EXPECT_TRUE(i.get<decltype(3)>() == 3);

    auto iValue = i.get<decltype(3)>();
    EXPECT_TRUE(iValue == 3);

    EXPECT_TRUE(peer(4).has_value());
    EXPECT_TRUE(peer(4).get<decltype(4)>() == 4);

    peer f = 6.2f;
    EXPECT_TRUE(f.has_value());
    EXPECT_TRUE(f.get<decltype(6.2f)>() == 6.2f);

    const float fValue = f.get<decltype(6.2f)>();
    EXPECT_TRUE(fValue == 6.2f);

    EXPECT_TRUE(peer(1.0f).has_value());
    EXPECT_TRUE(peer(1.0f).get<decltype(1.0f)>() == 1.0f);

    peer c = 'z';
    EXPECT_TRUE(c.has_value());
    EXPECT_TRUE(c.get<decltype('z')>() == 'z');

    EXPECT_TRUE(peer('z').has_value());
    EXPECT_TRUE(peer('z').get<decltype('z')>() == 'z');
}

TEST(Peer, BasicTypes_Move) {
    peer i = 3;
    EXPECT_TRUE(i.has_value());

    peer f = 6.2f;
    EXPECT_TRUE(f.has_value());

    f = std::move(i);
    EXPECT_FALSE(i.has_value());

    EXPECT_TRUE(f.has_value());
    EXPECT_TRUE(f.get<decltype(3)>() == 3);
}

TEST(Peer, SmallType) {
    struct T {
        T(int32_t* p_) : p(p_) {
            (*p)++;
        }

        T(T&& t) noexcept : p(t.p) {
            (*p)++;
        }

        ~T() {
            (*p)--;
        }

        T(const T&) = delete;
        T& operator=(const T&) = delete;

        int32_t* p;
    };

    int32_t p = 0;

    {
        peer u1 = peer(T(&p));
        EXPECT_EQ(p, 1);

        auto u2(std::move(u1));
        EXPECT_EQ(p, 1);
    }

    EXPECT_EQ(p, 0);
}

// peer is not able to receive large types, unless we increase storage_t
// capacity.
//TEST(Peer, LargeType) {
//    TestType t1;
//    peer u1 = peer(std::move(t1));
//    EXPECT_TRUE(u1.has_value());
//
//    //TestType should be moved into owning peer
//    EXPECT_EQ(u1.get<TestType>().i1, 1);
//
//    auto u2(std::move(u1));
//    EXPECT_FALSE(u1.has_value());
//
//    //TestType should not be moved when owning peer is moved;
//    EXPECT_EQ(u2.get<TestType>().i1, 1);
//
//    //TestType should be moved out of owning peer
//    // Note: two moves are involved in returning the moved value
//    //  First out of the peer, and then in the return statement
//    auto t2 = std::move(u2.get<TestType>());
//    EXPECT_FALSE(u2.has_value());
//    EXPECT_EQ(t2.i1, 3);
//}

TEST(Peer, Pointer) {
    auto t1 = new TestType();

    auto u1 = peer(std::move(t1));
    EXPECT_TRUE(u1.has_value());

    //Only the pointer should be moved
    TestType * t2 = u1.get<TestType *>();
    EXPECT_EQ(t2->i1, 0);

    peer u2(4);
    std::swap(u2, u1);

    EXPECT_TRUE(u1.has_value());

    EXPECT_TRUE(u2.has_value());

    t2 = u2.get<TestType *>();
    EXPECT_EQ(t2->i1, 0);
    delete t2;
}


TEST(Peer, UniquePtr) {
    auto t1 = std::make_unique<TestType>();
    auto u1 = peer(std::move(t1));

    EXPECT_EQ(t1.get(), nullptr);
    EXPECT_TRUE(u1.has_value());

    auto t2 = std::move(u1.take<TestType>());
    EXPECT_FALSE(u1.has_value());
    (void)t2;

    peer u2;
    TestType * t3 = new TestType();
    u2 = std::unique_ptr<TestType>(t3);
    EXPECT_TRUE(u2.has_value());
}

TEST(Peer, SharedPtr) {

    std::shared_ptr<int> shared(new int(3));
    std::weak_ptr<int> weak = shared;
    peer u1 = 0;

    EXPECT_EQ(weak.use_count(), 1);
    peer u2 = shared;
    EXPECT_EQ(weak.use_count(), 2);

    u1 = std::move(u2);
    EXPECT_EQ(weak.use_count(), 2);
    u2.swap(u1);
    EXPECT_EQ(weak.use_count(), 2);
    u2 = 0;
    EXPECT_EQ(weak.use_count(), 1);
    shared = nullptr;
    EXPECT_EQ(weak.use_count(), 0);
}
