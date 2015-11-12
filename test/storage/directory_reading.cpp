#include "storage.hpp"

#include <uv.h>

#include <mbgl/storage/default_file_source.hpp>
#include <mbgl/util/run_loop.hpp>

TEST_F(Storage, AssetReadDirectory) {
    SCOPED_TEST(ReadDirectory)

    using namespace mbgl;

#ifdef MBGL_ASSET_ZIP
    DefaultFileSource fs(nullptr, "test/fixtures/storage/assets.zip");
#else
    DefaultFileSource fs(nullptr);
#endif

    util::RunLoop loop(uv_default_loop());

    std::unique_ptr<FileRequest> req = fs.request({ Resource::Unknown, "asset://TEST_DATA/fixtures/storage" }, [&](Response res) {
        req.reset();
        ASSERT_NE(nullptr, res.error);
        EXPECT_EQ(Response::Error::Reason::NotFound, res.error->reason);
        EXPECT_EQ(false, res.stale);
        ASSERT_FALSE(res.data.get());
        EXPECT_EQ(0, res.expires);
        EXPECT_EQ(0, res.modified);
        EXPECT_EQ("", res.etag);
#ifdef MBGL_ASSET_ZIP
        EXPECT_EQ("No such file", res.error->message);
#elif MBGL_ASSET_FS
        EXPECT_EQ("illegal operation on a directory", res.error->message);
#endif

        loop.stop();
        ReadDirectory.finish();
    });

    uv_run(uv_default_loop(), UV_RUN_DEFAULT);
}
