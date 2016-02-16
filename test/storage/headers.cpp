#include "storage.hpp"
#include "../fixtures/fixture_log_observer.hpp"

#include <mbgl/util/http_header.hpp>

TEST_F(Storage, HTTPHeaderParsing) {
    using namespace mbgl;

    http::CacheControl cc;

    cc = http::CacheControl::parse(R"#()#");
    ASSERT_FALSE(bool(cc.maxAge));
    EXPECT_FALSE(cc.mustRevalidate);

    cc = http::CacheControl::parse(R"#(max-age =34)#");
    ASSERT_TRUE(bool(cc.maxAge));
    EXPECT_EQ(34, *cc.maxAge);
    EXPECT_FALSE(cc.mustRevalidate);

    cc = http::CacheControl::parse(R"#(,max-age=1)#");
    ASSERT_TRUE(bool(cc.maxAge));
    EXPECT_EQ(1, *cc.maxAge);
    EXPECT_FALSE(cc.mustRevalidate);

    cc = http::CacheControl::parse(R"#(max-age=-1)#");
    ASSERT_FALSE(bool(cc.maxAge));
    EXPECT_FALSE(cc.mustRevalidate);

    cc = http::CacheControl::parse(R"#(max-age=foo)#");
    ASSERT_FALSE(bool(cc.maxAge));
    EXPECT_FALSE(cc.mustRevalidate);

    cc = http::CacheControl::parse(R"#(max-age="34,max-age="22,max-age=28)#");
    ASSERT_TRUE(bool(cc.maxAge));
    EXPECT_EQ(28, *cc.maxAge);
    EXPECT_FALSE(cc.mustRevalidate);

    cc = http::CacheControl::parse(R"#(max-age=3,max-age="34)#");
    ASSERT_TRUE(bool(cc.maxAge));
    EXPECT_EQ(3, *cc.maxAge);
    EXPECT_FALSE(cc.mustRevalidate);

    cc = http::CacheControl::parse(R"#(max-age="\",max-age=4,")#");
    ASSERT_FALSE(bool(cc.maxAge));
    EXPECT_FALSE(cc.mustRevalidate);

    cc = http::CacheControl::parse(R"#(private, max-age=0, no-cache)#");
    ASSERT_TRUE(bool(cc.maxAge));
    EXPECT_EQ(0, *cc.maxAge);
    EXPECT_FALSE(cc.mustRevalidate);

    cc = http::CacheControl::parse(R"#(max-age=0, no-cache, no-store)#");
    ASSERT_TRUE(bool(cc.maxAge));
    EXPECT_EQ(0, *cc.maxAge);
    EXPECT_FALSE(cc.mustRevalidate);

    cc = http::CacheControl::parse(R"#(,   private   , max-bar=3 , no-cache,  "\,",,foo=",",,max-age=32)#");
    ASSERT_TRUE(bool(cc.maxAge));
    EXPECT_EQ(32, *cc.maxAge);
    EXPECT_FALSE(cc.mustRevalidate);

    cc = http::CacheControl::parse(R"#(max-age=3600, must-revalidate)#");
    ASSERT_TRUE(bool(cc.maxAge));
    EXPECT_EQ(3600, *cc.maxAge);
    EXPECT_TRUE(cc.mustRevalidate);

    cc = http::CacheControl::parse(R"#(no-cache="Expires,Via",max-age=3600, must-revalidate)#");
    ASSERT_TRUE(bool(cc.maxAge));
    EXPECT_EQ(3600, *cc.maxAge);
    EXPECT_TRUE(cc.mustRevalidate);
}
