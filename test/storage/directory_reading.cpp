#include "storage.hpp"

#include <uv.h>

#include <mbgl/storage/default/default_file_source.hpp>

TEST_F(Storage, ReadDirectory) {
    SCOPED_TEST(ReadDirectory)

    using namespace mbgl;

    DefaultFileSource fs(nullptr, uv_default_loop());

    fs.request({ Resource::Unknown, "asset://TEST_DATA/fixtures/storage" }, uv_default_loop(), [&](const Response &res) {
        EXPECT_EQ(Response::Error, res.status);
        EXPECT_EQ(0ul, res.data.size());
        EXPECT_EQ(0, res.expires);
        EXPECT_EQ(0, res.modified);
        EXPECT_EQ("", res.etag);
        EXPECT_EQ("illegal operation on a directory", res.message);
        ReadDirectory.finish();
    });

    uv_run(uv_default_loop(), UV_RUN_DEFAULT);
}
