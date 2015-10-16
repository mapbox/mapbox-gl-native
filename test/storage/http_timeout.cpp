#include "storage.hpp"

#include <uv.h>

#include <mbgl/storage/default_file_source.hpp>
#include <mbgl/storage/network_status.hpp>


TEST_F(Storage, HTTPTimeout) {
    SCOPED_TEST(HTTPTimeout)

    using namespace mbgl;

    DefaultFileSource fs(nullptr);

    int counter = 0;

    const Resource resource { Resource::Unknown, "http://127.0.0.1:3000/test?cachecontrol=max-age=1" };
    Request* req = fs.request(resource, uv_default_loop(), [&](const Response &res) {
        counter++;
        EXPECT_EQ(Response::Successful, res.status);
        EXPECT_EQ(false, res.stale);
        ASSERT_TRUE(res.data.get());
        EXPECT_EQ("Hello World!", *res.data);
        EXPECT_LT(0, res.expires);
        EXPECT_EQ(0, res.modified);
        EXPECT_EQ("", res.etag);
        EXPECT_EQ("", res.message);
        if (counter == 4) {
            fs.cancel(req);
            HTTPTimeout.finish();
        }
    });

    uv_run(uv_default_loop(), UV_RUN_DEFAULT);

    EXPECT_EQ(4, counter);
}
