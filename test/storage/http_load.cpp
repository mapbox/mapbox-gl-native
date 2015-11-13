#include "storage.hpp"

#include <mbgl/storage/default_file_source.hpp>
#include <mbgl/util/chrono.hpp>
#include <mbgl/util/run_loop.hpp>

TEST_F(Storage, HTTPLoad) {
    SCOPED_TEST(HTTPLoad)

    using namespace mbgl;

    util::RunLoop loop;
    DefaultFileSource fs(nullptr);

    const int concurrency = 50;
    const int max = 10000;
    int number = 1;

    std::unique_ptr<FileRequest> reqs[concurrency];

    std::function<void(int)> req = [&](int i) {
        const auto current = number++;
        reqs[i] = fs.request({ Resource::Unknown,
                     std::string("http://127.0.0.1:3000/load/") + std::to_string(current) },
                   [&, i, current](Response res) {
            reqs[i].reset();
            EXPECT_EQ(nullptr, res.error);
            EXPECT_EQ(false, res.stale);
            ASSERT_TRUE(res.data.get());
            EXPECT_EQ(std::string("Request ") +  std::to_string(current), *res.data);
            EXPECT_EQ(Seconds::zero(), res.expires);
            EXPECT_EQ(Seconds::zero(), res.modified);
            EXPECT_EQ("", res.etag);

            if (number <= max) {
                req(i);
            } else if (current == max) {
                loop.stop();
                HTTPLoad.finish();
            }
        });
    };


    for (int i = 0; i < concurrency; i++) {
        req(i);
    }

    loop.run();
}
