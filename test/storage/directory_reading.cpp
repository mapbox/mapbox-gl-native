#include "storage.hpp"

#include <mbgl/storage/default_file_source.hpp>
#include <mbgl/util/chrono.hpp>
#include <mbgl/util/run_loop.hpp>

TEST_F(Storage, AssetReadDirectory) {
    SCOPED_TEST(ReadDirectory)

    using namespace mbgl;

    util::RunLoop loop;

#ifdef MBGL_ASSET_ZIP
    DefaultFileSource fs(nullptr, "test/fixtures/storage/assets.zip");
#else
    DefaultFileSource fs(nullptr);
#endif

    std::unique_ptr<FileRequest> req = fs.request({ Resource::Unknown, "asset://TEST_DATA/fixtures/storage" }, [&](Response res) {
        req.reset();
        ASSERT_NE(nullptr, res.error);
        EXPECT_EQ(Response::Error::Reason::NotFound, res.error->reason);
        EXPECT_EQ(false, res.stale);
        ASSERT_FALSE(res.data.get());
        EXPECT_EQ(Seconds::zero(), res.expires);
        EXPECT_EQ(Seconds::zero(), res.modified);
        EXPECT_EQ("", res.etag);
#ifdef MBGL_ASSET_ZIP
        EXPECT_EQ("Could not stat file in zip archive", res.error->message);
#elif MBGL_ASSET_FS
        EXPECT_EQ("Is a directory", res.error->message);
#endif

        loop.stop();
        ReadDirectory.finish();
    });

    loop.run();
}
