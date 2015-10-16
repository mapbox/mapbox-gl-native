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

    Request* reqs[concurrency];

    std::function<void(int)> req = [&](int i) {
        const auto current = number++;
        reqs[i] = fs.request({ Resource::Unknown,
                     std::string("http://127.0.0.1:3000/load/") + std::to_string(current) },
                   uv_default_loop(), [&, i, current](const Response &res) {
            fs.cancel(reqs[i]);
            reqs[i] = nullptr;
            EXPECT_EQ(Response::Successful, res.status);
            EXPECT_EQ(false, res.stale);
            ASSERT_TRUE(res.data.get());
            EXPECT_EQ(std::string("Request ") +  std::to_string(current), *res.data);
            EXPECT_EQ(0, res.expires);
            EXPECT_EQ(0, res.modified);
            EXPECT_EQ("", res.etag);
            EXPECT_EQ("", res.message);

            if (number <= max) {
                req(i);
            } else if (current == max) {
                HTTPLoad.finish();
            }
        });
    };


    for (int i = 0; i < concurrency; i++) {
        req(i);
    }

    uv_run(uv_default_loop(), UV_RUN_DEFAULT);
}
