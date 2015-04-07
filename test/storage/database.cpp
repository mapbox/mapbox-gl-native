#include "storage.hpp"
#include "../fixtures/fixture_log_observer.hpp"

#include <future>

#include <mbgl/storage/default/sqlite_cache.hpp>
#include <mbgl/storage/resource.hpp>
#include <mbgl/storage/response.hpp>

#include <fcntl.h>

class ScopedTest {
public:
    ScopedTest(std::function<void()> destructor_) : destructor(destructor_) {}
    ScopedTest() {}

    void finish() {
        promise.set_value();
    }

    ~ScopedTest() {
        promise.get_future().get();
        if (destructor) {
            destructor();
        }
    }

private:
    const std::function<void()> destructor;
    std::promise<void> promise;
};

TEST_F(Storage, DatabaseDoesNotExist) {
    using namespace mbgl;

    Log::setObserver(util::make_unique<FixtureLogObserver>());

    ScopedTest test([&] {
        auto observer = Log::removeObserver();
        EXPECT_EQ(1ul, dynamic_cast<FixtureLogObserver*>(observer.get())->count({ EventSeverity::Error, Event::Database, 14, "unable to open database file" }));
    });

    SQLiteCache cache("test/fixtures/404/cache.db");

    cache.get({ Resource::Unknown, "mapbox://test" }, [&] (std::unique_ptr<Response> res) {
        EXPECT_EQ(nullptr, res.get());
        test.finish();
    });
}

void createDir(const char* name) {
    const int ret = mkdir(name, 0755);
    if (ret == -1) {
        ASSERT_EQ(EEXIST, errno);
    } else {
        ASSERT_EQ(0, ret);
    }
}

void deleteFile(const char* name) {
    const int ret = unlink(name);
    if (ret == -1) {
        ASSERT_EQ(ENOENT, errno);
    } else {
        ASSERT_EQ(0, ret);
    }
}

TEST_F(Storage, DatabaseCreate) {
    using namespace mbgl;

    createDir("test/fixtures/database");
    deleteFile("test/fixtures/database/cache.db");

    Log::setObserver(util::make_unique<FixtureLogObserver>());

    ScopedTest test([&] {
        Log::removeObserver();
    });

    SQLiteCache cache("test/fixtures/database/cache.db");

    cache.get({ Resource::Unknown, "mapbox://test" }, [&] (std::unique_ptr<Response> res) {
        EXPECT_EQ(nullptr, res.get());
        test.finish();
    });
}

class FileLock {
public:
    FileLock(const std::string& path) {
        fd = open(path.c_str(), O_RDONLY | O_NOCTTY | O_CREAT, 0666);
        if (fd <= 0) {
            throw std::runtime_error("Could not open file");
        }
        lock();
    }

    void lock() {
        if (0 != flock(fd, LOCK_EX)) {
            throw std::runtime_error("Could not lock file");
        }
    }

    void unlock() {
        if (0 != flock(fd, LOCK_UN)) {
            throw std::runtime_error("Could not unlock file");
        }
    }

    ~FileLock() {
        unlock();
    }

private:
    int fd;
};

TEST_F(Storage, DatabaseLockedRead) {
    using namespace mbgl;

    // Create a locked file.
    createDir("test/fixtures/database");
    deleteFile("test/fixtures/database/locked.db");
    FileLock guard("test/fixtures/database/locked.db");

    auto cache = util::make_unique<SQLiteCache>("test/fixtures/database/locked.db");

    std::promise<void> promise;

    {
        // First request should fail.
        Log::setObserver(util::make_unique<FixtureLogObserver>());
        promise = {};
        cache->get({ Resource::Unknown, "mapbox://test" }, [&] (std::unique_ptr<Response> res) {
            EXPECT_EQ(nullptr, res.get());
            promise.set_value();
        });
        promise.get_future().get();

        // Make sure that we got a few "database locked" errors
        auto observer = Log::removeObserver();
        auto flo = dynamic_cast<FixtureLogObserver*>(observer.get());
        EXPECT_EQ(2ul, flo->count({ EventSeverity::Error, Event::Database, 5, "database is locked" }));
    }

    // Then, unlock the file and try again.
    guard.unlock();

    {
        // First, try getting a file (the cache value should not exist).
        Log::setObserver(util::make_unique<FixtureLogObserver>());
        promise = {};

        cache->get({ Resource::Unknown, "mapbox://test" }, [&] (std::unique_ptr<Response> res) {
            EXPECT_EQ(nullptr, res.get());
            promise.set_value();
        });
        promise.get_future().get();

        // Make sure that we got a no errors
        Log::removeObserver();
    }

    // Explicitly delete the Cache now.
    cache.reset();
}



TEST_F(Storage, DatabaseLockedWrite) {
    using namespace mbgl;

    // Create a locked file.
    createDir("test/fixtures/database");
    deleteFile("test/fixtures/database/locked.db");
    FileLock guard("test/fixtures/database/locked.db");

    auto cache = util::make_unique<SQLiteCache>("test/fixtures/database/locked.db");

    std::promise<void> promise;

    {
        // Adds a file (which should fail).
        Log::setObserver(util::make_unique<FixtureLogObserver>());
        promise = {};
        auto response = std::make_shared<Response>();
        cache->put({ Resource::Unknown, "mapbox://test" }, response, FileCache::Hint::Full);
        cache->get({ Resource::Unknown, "mapbox://test" }, [&] (std::unique_ptr<Response> res) {
            EXPECT_EQ(nullptr, res.get());
            promise.set_value();
        });
        promise.get_future().get();

        auto observer = Log::removeObserver();
        auto flo = dynamic_cast<FixtureLogObserver*>(observer.get());
        EXPECT_EQ(4ul, flo->count({ EventSeverity::Error, Event::Database, 5, "database is locked" }));
    }

    // Then, unlock the file and try again.
    guard.unlock();

    {
        // Then, set a file and obtain it again.
        Log::setObserver(util::make_unique<FixtureLogObserver>());
        promise = {};

        auto response = std::make_shared<Response>();
        response->data = "Demo";
        cache->put({ Resource::Unknown, "mapbox://test" }, response, FileCache::Hint::Full);
        cache->get({ Resource::Unknown, "mapbox://test" }, [&] (std::unique_ptr<Response> res) {
            EXPECT_NE(nullptr, res.get());
            EXPECT_EQ("Demo", res->data);
            promise.set_value();
        });
        promise.get_future().get();

        // Make sure that we got a no errors
        Log::removeObserver();
    }

    // Explicitly delete the Cache now.
    cache.reset();
}




TEST_F(Storage, DatabaseLockedRefresh) {
    using namespace mbgl;

    // Create a locked file.
    createDir("test/fixtures/database");
    deleteFile("test/fixtures/database/locked.db");

    auto cache = util::make_unique<SQLiteCache>("test/fixtures/database/locked.db");

    // Then, lock the file and try again.
    FileLock guard("test/fixtures/database/locked.db");

    std::promise<void> promise;

    {
        // Adds a file.
        Log::setObserver(util::make_unique<FixtureLogObserver>());
        promise = {};
        auto response = std::make_shared<Response>();
        response->data = "Demo";
        cache->put({ Resource::Unknown, "mapbox://test" }, response, FileCache::Hint::Full);
        cache->get({ Resource::Unknown, "mapbox://test" }, [&] (std::unique_ptr<Response> res) {
            EXPECT_EQ(nullptr, res.get());
            promise.set_value();
        });
        promise.get_future().get();

        auto observer = Log::removeObserver();
        auto flo = dynamic_cast<FixtureLogObserver*>(observer.get());
        EXPECT_EQ(4ul, flo->count({ EventSeverity::Error, Event::Database, 5, "database is locked" }));
    }

    {
        // Then, try to refresh it.
        Log::setObserver(util::make_unique<FixtureLogObserver>());
        promise = {};

        auto response = std::make_shared<Response>();
        response->data = "Demo";
        cache->put({ Resource::Unknown, "mapbox://test" }, response, FileCache::Hint::Refresh);
        cache->get({ Resource::Unknown, "mapbox://test" }, [&] (std::unique_ptr<Response> res) {
            EXPECT_EQ(nullptr, res.get());
            promise.set_value();
        });
        promise.get_future().get();

        // Make sure that we got the right errors.
        auto observer = Log::removeObserver();
        auto flo = dynamic_cast<FixtureLogObserver*>(observer.get());
        EXPECT_EQ(4ul, flo->count({ EventSeverity::Error, Event::Database, 5, "database is locked" }));
    }

    // Explicitly delete the Cache now.
    cache.reset();
}



TEST_F(Storage, DatabaseDeleted) {
    using namespace mbgl;

    // Create a locked file.
    createDir("test/fixtures/database");
    deleteFile("test/fixtures/database/locked.db");

    auto cache = util::make_unique<SQLiteCache>("test/fixtures/database/locked.db");

    std::promise<void> promise;

    {
        // Adds a file.
        Log::setObserver(util::make_unique<FixtureLogObserver>());
        promise = {};
        auto response = std::make_shared<Response>();
        response->data = "Demo";
        cache->put({ Resource::Unknown, "mapbox://test" }, response, FileCache::Hint::Full);
        cache->get({ Resource::Unknown, "mapbox://test" }, [&] (std::unique_ptr<Response> res) {
            EXPECT_NE(nullptr, res.get());
            EXPECT_EQ("Demo", res->data);
            promise.set_value();
        });
        promise.get_future().get();

        Log::removeObserver();
    }

    deleteFile("test/fixtures/database/locked.db");

    {
        // Adds a file.
        Log::setObserver(util::make_unique<FixtureLogObserver>());
        promise = {};
        auto response = std::make_shared<Response>();
        response->data = "Demo";
        cache->put({ Resource::Unknown, "mapbox://test" }, response, FileCache::Hint::Full);
        cache->get({ Resource::Unknown, "mapbox://test" }, [&] (std::unique_ptr<Response> res) {
            EXPECT_NE(nullptr, res.get());
            EXPECT_EQ("Demo", res->data);
            promise.set_value();
        });
        promise.get_future().get();

        auto observer = Log::removeObserver();
        auto flo = dynamic_cast<FixtureLogObserver*>(observer.get());
        EXPECT_EQ(1ul, flo->count({ EventSeverity::Error, Event::Database, 8, "attempt to write a readonly database" }));
    }

    // Explicitly delete the Cache now.
    cache.reset();
}
