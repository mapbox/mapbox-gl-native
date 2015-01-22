#include "storage.hpp"

#include <uv.h>

#include <mbgl/storage/default/default_file_source.hpp>

TEST_F(Storage, HTTPLoad) {
    SCOPED_TEST(HTTPLoad)

    using namespace mbgl;

    DefaultFileSource fs(nullptr, uv_default_loop());

    const int concurrency = 50;
    const int max = 10000;
    int number = 1;

    std::function<void()> req = [&]() {
        const auto current = number++;
        fs.request({ Resource::Unknown, std::string("http://127.0.0.1:3000/load/") +  std::to_string(current) }, uv_default_loop(), [&, current](const Response &res) {
            EXPECT_EQ(res.status, Response::Successful);
            EXPECT_EQ(res.data, std::string("Request ") +  std::to_string(current));
            EXPECT_EQ(res.expires, 0);
            EXPECT_EQ(res.modified, 0);
            EXPECT_EQ(res.etag, "");
            EXPECT_EQ(res.message, "");

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
