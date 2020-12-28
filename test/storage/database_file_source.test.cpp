#include <mbgl/storage/file_source_manager.hpp>
#include <mbgl/storage/resource.hpp>
#include <mbgl/storage/resource_options.hpp>
#include <mbgl/test/util.hpp>
#include <mbgl/util/run_loop.hpp>
#include <mbgl/util/timer.hpp>

#include <gtest/gtest.h>

using namespace mbgl;

TEST(DatabaseFileSource, PauseResume) {
    util::RunLoop loop;

    std::shared_ptr<FileSource> dbfs =
        FileSourceManager::get()->getFileSource(FileSourceType::Database, ResourceOptions{});
    dbfs->pause();

    const Resource res{Resource::Unknown, "http://127.0.0.1:3000/test", {}, Resource::LoadingMethod::CacheOnly};
    auto req = dbfs->request(res, [&](const Response&) { loop.stop(); });

    util::Timer resumeTimer;
    resumeTimer.start(Milliseconds(5), Duration::zero(), [dbfs] { dbfs->resume(); });

    loop.run();
}

TEST(DatabaseFileSource, VolatileResource) {
    util::RunLoop loop;

    std::shared_ptr<FileSource> dbfs =
        FileSourceManager::get()->getFileSource(FileSourceType::Database, ResourceOptions{});

    Resource resource{Resource::Unknown, "http://127.0.0.1:3000/test", {}, Resource::LoadingMethod::CacheOnly};
    Response response{};
    response.data = std::make_shared<std::string>("Cached value");
    std::unique_ptr<mbgl::AsyncRequest> req;

    dbfs->forward(resource, response, [&] {
        req = dbfs->request(resource, [&](Response res1) {
            EXPECT_EQ(nullptr, res1.error);
            ASSERT_TRUE(res1.data.get());
            EXPECT_FALSE(res1.noContent);
            EXPECT_EQ("Cached value", *res1.data);
            resource.storagePolicy = Resource::StoragePolicy::Volatile;
            req = dbfs->request(resource, [&](Response res2) {
                req.reset();
                ASSERT_TRUE(res2.error.get());
                EXPECT_TRUE(res2.noContent);
                EXPECT_EQ(Response::Error::Reason::NotFound, res2.error->reason);
                EXPECT_EQ("Not found in offline database", res2.error->message);
                loop.stop();
            });
        });
    });
    loop.run();
}