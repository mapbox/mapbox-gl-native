#include "../util.hpp"

#include <uv.h>

#include <mbgl/storage/default/default_file_source.hpp>

TEST(Storage, EmptyFile) {
    SCOPED_TEST(EmptyFile)

    using namespace mbgl;

    DefaultFileSource fs(nullptr, uv_default_loop());

    fs.request({ Resource::Unknown, "asset://test/fixtures/storage/empty" }, uv_default_loop(), [&](const Response &res) {
        EXPECT_EQ(res.status, Response::Successful);
        EXPECT_EQ(res.data.size(), 0ul);
        EXPECT_EQ(res.expires, 0);
        EXPECT_GT(res.modified, 0);
        EXPECT_NE(res.etag, "");
        EXPECT_EQ(res.message, "");
        EmptyFile.finish();
    });

    uv_run(uv_default_loop(), UV_RUN_DEFAULT);
}

TEST(Storage, NonExistentFile) {
    SCOPED_TEST(NonExistentFile)

    using namespace mbgl;

    DefaultFileSource fs(nullptr, uv_default_loop());

    fs.request({ Resource::Unknown, "asset://test/fixtures/storage/does_not_exist" }, uv_default_loop(), [&](const Response &res) {
        EXPECT_EQ(res.status, Response::Error);
        EXPECT_EQ(res.data.size(), 0ul);
        EXPECT_EQ(res.expires, 0);
        EXPECT_EQ(res.modified, 0);
        EXPECT_EQ(res.etag, "");
        EXPECT_EQ(res.message, "no such file or directory");
        NonExistentFile.finish();
    });

    uv_run(uv_default_loop(), UV_RUN_DEFAULT);
}
