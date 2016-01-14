#include "storage.hpp"

#include <mbgl/storage/sqlite_cache.hpp>
#include <mbgl/storage/resource.hpp>
#include <mbgl/util/run_loop.hpp>

TEST_F(Storage, CacheShared) {
    SCOPED_TEST(CacheShared)
    using namespace mbgl;

    util::RunLoop loop;

    // Check that we're getting two different caches when we request different paths.
    auto memory = SQLiteCache::getShared();
    auto file = SQLiteCache::getShared("test/fixtures/database/cache.db");
    EXPECT_NE(memory.get(), file.get());
    EXPECT_EQ(memory.get(), SQLiteCache::getShared().get());

    // Store a response into the memory file cache, then delete the last reference.
    const Resource resource { Resource::Kind::Unknown, "http://example.com" };
    memory->put(resource, Response());
    memory.reset();

    // Now check that the original memory file cache has been destructed and that it doesn't contain
    // the information we put into it.
    memory = SQLiteCache::getShared();
    auto req = memory->get(resource, [&](std::unique_ptr<Response> res) {
        EXPECT_FALSE(res.get());
        CacheShared.finish();
        loop.stop();
    });

    loop.run();
}
