#include "storage.hpp"

#include <uv.h>

#include <mbgl/storage/default/default_file_source.hpp>

TEST_F(Storage, HTTPCoalescing) {
    SCOPED_TEST(HTTPCoalescing)

    static int counter = 0;
    const static int total = 4;

    using namespace mbgl;

    DefaultFileSource fs(nullptr, uv_default_loop());

    static const Response *reference = nullptr;

    const auto complete = [&](const Response &res) {
        counter++;

        // Make sure all of the Response objects are the same.
        if (!reference) {
            reference = &res;
        } else {
            EXPECT_EQ(reference, &res);
        }

        EXPECT_EQ(res.status, Response::Successful);
        EXPECT_EQ(res.data, "Hello World!");
        EXPECT_EQ(res.expires, 0);
        EXPECT_EQ(res.modified, 0);
        EXPECT_EQ(res.etag, "");
        EXPECT_EQ(res.message, "");

        if (counter >= total) {
            HTTPCoalescing.finish();
        }
    };

    const Resource resource { Resource::Unknown, "http://127.0.0.1:3000/test" };

    for (int i = 0; i < total; i++) {
        fs.request(resource, uv_default_loop(), complete);
    }

    uv_run(uv_default_loop(), UV_RUN_DEFAULT);
}
