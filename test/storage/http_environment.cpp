#include "storage.hpp"

#include <uv.h>

#include <mbgl/storage/default_file_source.hpp>
#include <mbgl/storage/network_status.hpp>
#include <mbgl/util/thread.hpp>

#include <cmath>

TEST_F(Storage, HTTPCancelEnvironment) {
    SCOPED_TEST(HTTPRetainedEnvironment)
    SCOPED_TEST(HTTPCanceledEnvironment)

    using namespace mbgl;

    util::Thread<DefaultFileSource> fs(nullptr);

    // Create two fake environment pointers. The FileSource implementation treats these as opaque
    // pointers and doesn't reach into them.
    auto &env1 = *reinterpret_cast<const Environment *>(1);
    auto &env2 = *reinterpret_cast<const Environment *>(2);

    const Resource resource { Resource::Unknown, "http://127.0.0.1:3000/delayed" };

    // Environment 1
    fs->request(resource, uv_default_loop(), env1, [&](const Response &res) {
        // This environment gets aborted below. This means the request is marked as failing and
        // will return an error here.
        EXPECT_EQ(Response::Error, res.status);
        EXPECT_EQ("", res.data);
        EXPECT_EQ(0, res.expires);
        EXPECT_EQ(0, res.modified);
        EXPECT_EQ("", res.etag);
        EXPECT_EQ("Environment is terminating", res.message);
        HTTPCanceledEnvironment.finish();
    });

    // Environment 2
    fs->request(resource, uv_default_loop(), env2, [&](const Response &res) {
        // The same request as above, but in a different environment which doesn't get aborted. This
        // means the request should succeed.
        EXPECT_EQ(Response::Successful, res.status);
        EXPECT_EQ("Response", res.data);
        EXPECT_EQ(0, res.expires);
        EXPECT_EQ(0, res.modified);
        EXPECT_EQ("", res.etag);
        EXPECT_EQ("", res.message);
        HTTPRetainedEnvironment.finish();
    });

    fs->abort(env1);

    uv_run(uv_default_loop(), UV_RUN_DEFAULT);
}
