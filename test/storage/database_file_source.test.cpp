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

    auto dbfs = FileSourceManager::get()->getFileSource(FileSourceType::Database, ResourceOptions{});
    dbfs->pause();

    const Resource res{Resource::Unknown, "http://127.0.0.1:3000/test", {}, Resource::LoadingMethod::CacheOnly};
    auto req = dbfs->request(res, [&](const Response&) { loop.stop(); });

    util::Timer resumeTimer;
    resumeTimer.start(Milliseconds(5), Duration::zero(), [dbfs] { dbfs->resume(); });

    loop.run();
}