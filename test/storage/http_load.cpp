#include "storage.hpp"

#include <uv.h>

#include <mbgl/storage/default_file_source.hpp>

TEST_F(Storage, HTTPLoad) {
    SCOPED_TEST(HTTPLoad)

    using namespace mbgl;

    DefaultFileSource fs(nullptr);

    const int concurrency = 50;
    const int max = 10000;
    int number = 1;

    std::function<void()> req = [&]() {
        const auto current = number++;
        fs.request({ Resource::Unknown,
                     std::string("http://127.0.0.1:3000/load/") + std::to_string(current) },
                   uv_default_loop(), [&, current](const Response &res) {
            EXPECT_EQ(Response::Successful, res.status);
            EXPECT_EQ(std::string("Request ") +  std::to_string(current), res.data);
            EXPECT_EQ(0, res.expires);
            EXPECT_EQ(0, res.modified);
            EXPECT_EQ("", res.etag);
            EXPECT_EQ("", res.message);

            if (number <= max) {
                req();
            } else if (current == max) {
                HTTPLoad.finish();
            }
        });
    };


    for (int i = 0; i < concurrency; i++) {
        req();
    }

    uv_run(uv_default_loop(), UV_RUN_DEFAULT);
}
