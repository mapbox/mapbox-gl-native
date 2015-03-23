#include "storage.hpp"

#include <mbgl/map/environment.hpp>
#include <mbgl/storage/default_file_source.hpp>

#include <uv.h>

#include <thread>

TEST_F(Storage, HTTPCoalescing) {
    SCOPED_TEST(HTTPCoalescing)

    static int counter = 0;
    const static int total = 4;

    using namespace mbgl;

    DefaultFileSource fs(nullptr, uv_default_loop());

    Environment env(fs);
    EnvironmentScope scope(env, ThreadType::Test, TEST_CASE_NAME(), uv_default_loop());

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
        EXPECT_EQ("Hello World!", res.data);
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
        fs.request(resource, std::this_thread::get_id(), complete);
    }

    uv_run(uv_default_loop(), UV_RUN_DEFAULT);
}
