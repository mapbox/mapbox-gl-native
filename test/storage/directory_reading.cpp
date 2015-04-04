#include "storage.hpp"

#include <uv.h>

#include <mbgl/storage/default_file_source.hpp>

TEST_F(Storage, AssetReadDirectory) {
    SCOPED_TEST(ReadDirectory)

    using namespace mbgl;

#ifdef MBGL_ASSET_ZIP
    DefaultFileSource fs(nullptr, uv_default_loop(), "test/fixtures/storage/assets.zip");
#else
    DefaultFileSource fs(nullptr, uv_default_loop());
#endif

    auto &env = *static_cast<const Environment *>(nullptr);

    fs.request({ Resource::Unknown, "asset://TEST_DATA/fixtures/storage" }, uv_default_loop(),
               env, [&](const Response &res) {
        EXPECT_EQ(Response::Error, res.status);
        ASSERT_TRUE(!!res.data);
        EXPECT_EQ(0ul, res.data->size());
        EXPECT_EQ(0, res.expires);
        EXPECT_EQ(0, res.modified);
        EXPECT_EQ("", res.etag);
#ifdef MBGL_ASSET_ZIP
        EXPECT_EQ("No such file", res.message);
#elif MBGL_ASSET_FS
                            EXPECT_EQ("illegal operation on a directory", res.message);
#endif
        ReadDirectory.finish();
    });

    uv_run(uv_default_loop(), UV_RUN_DEFAULT);
}
