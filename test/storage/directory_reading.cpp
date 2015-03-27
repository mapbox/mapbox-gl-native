#include "storage.hpp"

#include <mbgl/map/environment.hpp>
#include <mbgl/storage/default_file_source.hpp>

#include <uv.h>

#include <thread>

TEST_F(Storage, AssetReadDirectory) {
    SCOPED_TEST(ReadDirectory);

    using namespace mbgl;

#ifdef MBGL_ASSET_ZIP
    DefaultFileSource fs(nullptr, uv_default_loop(), "test/fixtures/storage/assets.zip");
#else
    DefaultFileSource fs(nullptr, uv_default_loop());
#endif

    Environment env(fs);
    EnvironmentScope scope(env, ThreadType::Test, TEST_CASE_NAME(), uv_default_loop());

    fs.request({ Resource::Unknown, "asset://TEST_DATA/fixtures/storage" },
               std::this_thread::get_id(), [&](const Response& res) {
        EXPECT_EQ(Response::Error, res.status);
        EXPECT_EQ(0ul, res.data.size());
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
