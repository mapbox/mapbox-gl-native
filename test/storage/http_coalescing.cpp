#include "storage.hpp"

#include <uv.h>

#include <mbgl/storage/default_file_source.hpp>

TEST_F(Storage, HTTPCoalescing) {
    SCOPED_TEST(HTTPCoalescing)

    static int counter = 0;
    const static int total = 4;

    using namespace mbgl;

    DefaultFileSource fs(nullptr, uv_default_loop());

    auto &env = *static_cast<const Environment *>(nullptr);

    static const Response *reference = nullptr;

    const auto complete = [&](const Response &res) {
        counter++;

        // Make sure all of the Response objects are the same.
        if (!reference) {
            reference = &res;
        } else {
            EXPECT_EQ(&res, reference);
        }

        EXPECT_EQ(Response::Successful, res.status);
        ASSERT_TRUE(!!res.data);
        EXPECT_EQ("Hello World!", *res.data);
        EXPECT_EQ(0, res.expires);
        EXPECT_EQ(0, res.modified);
        EXPECT_EQ("", res.etag);
        EXPECT_EQ("", res.message);

        if (counter >= total) {
            HTTPCoalescing.finish();
        }
    };

    const Resource resource { Resource::Unknown, "http://127.0.0.1:3000/test" };

    for (int i = 0; i < total; i++) {
        fs.request(resource, uv_default_loop(), env, complete);
    }

    uv_run(uv_default_loop(), UV_RUN_DEFAULT);
}
