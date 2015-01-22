#include "storage.hpp"

#include <uv.h>

#include <mbgl/storage/default/default_file_source.hpp>

TEST_F(Storage, ReadDirectory) {
    SCOPED_TEST(ReadDirectory)

    using namespace mbgl;

    DefaultFileSource fs(nullptr, uv_default_loop());

    const auto dir = std::string { "asset://" } + mbgl::test::getBaseDirectory() + "/fixtures/storage";
    fs.request({ Resource::Unknown, dir }, uv_default_loop(), [&](const Response &res) {
        EXPECT_EQ(res.status, Response::Error);
        EXPECT_EQ(res.data.size(), 0ul);
        EXPECT_EQ(res.expires, 0);
        EXPECT_EQ(res.modified, 0);
        EXPECT_EQ(res.etag, "");
        EXPECT_EQ(res.message, "illegal operation on a directory");
        ReadDirectory.finish();
    });

    uv_run(uv_default_loop(), UV_RUN_DEFAULT);
}
