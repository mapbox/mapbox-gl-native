#include "storage.hpp"

#include <uv.h>

#include <mbgl/storage/default_file_source.hpp>
#include <mbgl/storage/sqlite_cache.hpp>

// Test for https://github.com/mapbox/mapbox-gl-native/issue/1369
//
// A request for http://example.com is made. This triggers a cache get. While the cache get is
// pending, the request is canceled. This removes it from pending. Then, still while the cache get
// is pending, a second request is made for the same resource. This adds an entry back to pending
// and queues another cache request, even though the first one is still pending. Now both cache
// requests resolve to misses, resulting in two HTTP requests for the same resource. The first one
// will notify as expected, the second one will have bound a DefaultFileRequest* in the lambda that
// gets invalidated by the first notify's pending.erase, and when it gets notified, the crash
// occurs.

TEST_F(Storage, HTTPIssue1369) {
    SCOPED_TEST(HTTPIssue1369)

    using namespace mbgl;

    SQLiteCache cache;
    DefaultFileSource fs(&cache);

    const Resource resource { Resource::Unknown, "http://127.0.0.1:3000/test" };

    auto req = fs.request(resource, uv_default_loop(), [&](const Response&) {
        ADD_FAILURE() << "Callback should not be called";
    });
    fs.cancel(req);
    fs.request(resource, uv_default_loop(), [&](const Response &res) {
        EXPECT_EQ(Response::Successful, res.status);
        EXPECT_EQ("Hello World!", res.data);
        EXPECT_EQ(0, res.expires);
        EXPECT_EQ(0, res.modified);
        EXPECT_EQ("", res.etag);
        EXPECT_EQ("", res.message);
        HTTPIssue1369.finish();
    });

    uv_run(uv_default_loop(), UV_RUN_DEFAULT);
}
