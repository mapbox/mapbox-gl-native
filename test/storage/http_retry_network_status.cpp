#include "storage.hpp"

#include <mbgl/util/uv_detail.hpp>

#include <mbgl/storage/default_file_source.hpp>
#include <mbgl/storage/network_status.hpp>


// Test for https://github.com/mapbox/mapbox-gl-native/issues/2123
//
// A request is made. While the request is in progress, the network status changes. This should
// trigger an immediate retry of all requests that are not in progress. This test makes sure that
// we don't accidentally double-trigger the request.

TEST_F(Storage, HTTPNetworkStatusChange) {
    SCOPED_TEST(HTTPNetworkStatusChange)

    using namespace mbgl;

    DefaultFileSource fs(nullptr);

    const Resource resource { Resource::Unknown, "http://127.0.0.1:3000/delayed" };

    // This request takes 200 milliseconds to answer.
    Request* req = fs.request(resource, uv_default_loop(), [&](const Response& res) {
         fs.cancel(req);
         EXPECT_EQ(Response::Successful, res.status);
         EXPECT_EQ(false, res.stale);
         ASSERT_TRUE(res.data.get());
         EXPECT_EQ("Response", *res.data);
         EXPECT_EQ(0, res.expires);
         EXPECT_EQ(0, res.modified);
         EXPECT_EQ("", res.etag);
         EXPECT_EQ("", res.message);
         HTTPNetworkStatusChange.finish();
    });

    // After 50 milliseconds, we're going to trigger a NetworkStatus change.
    uv::timer reachableTimer(uv_default_loop());
    reachableTimer.start(50, 0, [] () {
        mbgl::NetworkStatus::Reachable();
    });

    // This timer will keep the loop alive to make sure we would be getting a response in caes the
    // network status change triggered another change (which it shouldn't).
    uv::timer delayTimer(uv_default_loop());
    delayTimer.start(300, 0, [] () {});

    uv_run(uv_default_loop(), UV_RUN_DEFAULT);
}
