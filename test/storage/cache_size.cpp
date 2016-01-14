#include "storage.hpp"

#include <mbgl/storage/resource.hpp>
#include <mbgl/storage/response.hpp>
#include <mbgl/storage/sqlite_cache.hpp>
#include <mbgl/util/run_loop.hpp>
#include <mbgl/util/string.hpp>
#include <mbgl/util/time.hpp>
#include <mbgl/util/timer.hpp>

#include <memory>
#include <random>

bool tileIsCached(mbgl::SQLiteCache* cache, unsigned id) {
    using namespace mbgl;

    auto url = std::string("http://tile") + mbgl::util::toString(id);
    bool replied = false;

    std::unique_ptr<Response> response;
    auto callback = [&] (std::unique_ptr<Response> res) {
        replied = true;
        response = std::move(res);
    };

    Resource resource{ Resource::Kind::Tile, url };
    auto req = cache->get(resource, callback);

    while (!replied) {
        util::RunLoop::Get()->runOnce();
    }

    return response != nullptr;
}

void insertTile(mbgl::SQLiteCache* cache, unsigned id, uint64_t size) {
    using namespace mbgl;

    auto url = std::string("http://tile") + mbgl::util::toString(id);

    Response response;
    response.modified = toSeconds(SystemClock::now());
    response.expires = toSeconds(SystemClock::now()) + Seconds(5000);
    response.etag = url;

    auto data = std::make_shared<std::string>(size, 0);

    // Fill data with garbage so SQLite won't try to
    // optimize allocation by reusing pages.
    static std::mt19937 generator;
    std::generate_n(data->begin(), size, generator);

    response.data = data;

    Resource resource{ Resource::Kind::Tile, url };
    cache->put(resource, response, SQLiteCache::Hint::Full);
}

void refreshTile(mbgl::SQLiteCache* cache, unsigned id) {
    using namespace mbgl;

    auto url = std::string("http://tile") + mbgl::util::toString(id);

    Response response;
    response.modified = toSeconds(SystemClock::now());
    response.expires = toSeconds(SystemClock::now()) + Seconds(5000);

    Resource resource{ Resource::Kind::Tile, url };
    cache->put(resource, response, SQLiteCache::Hint::Refresh);
}

uint64_t cacheSize(mbgl::SQLiteCache* cache, unsigned entryCount, uint64_t entrySize) {
    uint64_t total = 0;

    for (unsigned i = 0; i < entryCount; ++i) {
        if (tileIsCached(cache, i)) {
            total += entrySize;
        }
    }

    return total;
}

TEST_F(Storage, CacheEntrySizeLimit) {
    using namespace mbgl;

    util::RunLoop loop;
    SQLiteCache cache(":memory:");

    const uint64_t entrySize = 5 * 1024 * 1024; // 5 MB

    insertTile(&cache, 0, entrySize);
    EXPECT_TRUE(tileIsCached(&cache, 0));

    insertTile(&cache, 1, entrySize + 1);
    EXPECT_FALSE(tileIsCached(&cache, 1));

    insertTile(&cache, 2, entrySize - 1);
    EXPECT_TRUE(tileIsCached(&cache, 2));

    // Setting a new size should not delete existing entries.
    cache.setMaximumCacheEntrySize(entrySize / 2);
    EXPECT_TRUE(tileIsCached(&cache, 2));

    insertTile(&cache, 3, entrySize / 2 - 1);
    EXPECT_TRUE(tileIsCached(&cache, 3));

    insertTile(&cache, 4, entrySize);
    EXPECT_FALSE(tileIsCached(&cache, 4));

    cache.setMaximumCacheEntrySize(entrySize * 2);
    insertTile(&cache, 5, entrySize);
    EXPECT_TRUE(tileIsCached(&cache, 5));
}

TEST_F(Storage, CacheSizeSetNewLimit) {
    using namespace mbgl;

    util::RunLoop loop;
    SQLiteCache cache(":memory:");

    const unsigned entryCount = 800;
    const uint64_t entrySize = 10 * 1024; // 10 KB

    cache.setMaximumCacheEntrySize(entrySize + 1);

    // Cache size defaults to unlimited, all these
    // inserts should work.
    for (unsigned i = 0; i < entryCount; ++i) {
        insertTile(&cache, i, entrySize);
    }

    for (unsigned i = 0; i < entryCount; ++i) {
        EXPECT_TRUE(tileIsCached(&cache, i));
    }

    uint64_t expectedCacheSize = entryCount * entrySize;
    EXPECT_EQ(cacheSize(&cache, entryCount, entrySize), expectedCacheSize);

    // Setting a new size should remove records until the new
    // size limit is satisfied.
    cache.setMaximumCacheSize(expectedCacheSize / 2);
    EXPECT_LT(cacheSize(&cache, entryCount, entrySize), expectedCacheSize / 2);

    // Cache size 1 should practically clean the cache and
    // prevent adding any record, although it makes no sense
    // to use such size limit IRL.
    cache.setMaximumCacheSize(1);
    EXPECT_EQ(cacheSize(&cache, entryCount, entrySize), 0);

    insertTile(&cache, 1000, entrySize);
    EXPECT_FALSE(tileIsCached(&cache, 1000));

    // Zero should be treated as unlimited.
    cache.setMaximumCacheSize(0);

    for (unsigned i = 0; i < entryCount; ++i) {
        insertTile(&cache, i, entrySize);
    }

    EXPECT_EQ(cacheSize(&cache, entryCount, entrySize), expectedCacheSize);
}

TEST_F(Storage, CacheSizePruneLeastAccessed) {
    using namespace mbgl;

    util::RunLoop loop;
    SQLiteCache cache(":memory:");

    const unsigned entryCount = 400;
    const uint64_t entrySize = 10 * 1024; // 10 KB

    cache.setMaximumCacheEntrySize(entrySize + 1);
    cache.setMaximumCacheSize(entrySize * 350);

    for (unsigned i = 0; i < entryCount; ++i) {
        insertTile(&cache, i, entrySize);

        if (i == entryCount / 2) {
            // We need to sleep for 1s here because
            // that is the time resolution for the
            // `accessed` time. Then we 'ping' the
            // entry, that should update the
            // `accessed` time, so it won't get
            // pruned when we need more space.
            bool done = false;

            util::Timer timer;
            timer.start(std::chrono::milliseconds(1300),
                        Duration::zero(),
                        [&done] { done = true; });

            while (!done) {
                loop.runOnce();
            }

            EXPECT_TRUE(tileIsCached(&cache, 7));

            // Refresh should also update the `accessed`
            // time of a tile.
            refreshTile(&cache, 9);
        }
    }

    EXPECT_FALSE(tileIsCached(&cache, 6));
    EXPECT_FALSE(tileIsCached(&cache, 8));
    EXPECT_FALSE(tileIsCached(&cache, 10));

    EXPECT_TRUE(tileIsCached(&cache, 7));
    EXPECT_TRUE(tileIsCached(&cache, 9));
}

TEST_F(Storage, CacheSizeStress) {
    using namespace mbgl;

    util::RunLoop loop;
    SQLiteCache cache(":memory:");

    const unsigned entryCount = 2000;
    const uint64_t entrySize = 10 * 1024; // 10 KB

    cache.setMaximumCacheEntrySize(entrySize + 1);
    cache.setMaximumCacheSize(entrySize * 300);

    for (unsigned i = 0; i < entryCount; ++i) {
        insertTile(&cache, i, entrySize);
    }

    // Should not be in the cache as they were
    // first inserted.
    EXPECT_FALSE(tileIsCached(&cache, 0));
    EXPECT_FALSE(tileIsCached(&cache, 99));
    EXPECT_FALSE(tileIsCached(&cache, 199));
    EXPECT_FALSE(tileIsCached(&cache, 299));
    EXPECT_FALSE(tileIsCached(&cache, 399));

    EXPECT_TRUE(tileIsCached(&cache, entryCount - 1));

    EXPECT_LT(cacheSize(&cache, entryCount, entrySize), entrySize * 300);
}
