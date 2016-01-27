#include "storage.hpp"

#include <mbgl/storage/online_file_source.hpp>
#include <mbgl/util/chrono.hpp>
#include <mbgl/util/run_loop.hpp>

TEST_F(Storage, HTTPLoad) {
    SCOPED_TEST(HTTPLoad)

    using namespace mbgl;

    util::RunLoop loop;
    OnlineFileSource fs;

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
            ASSERT_TRUE(res.data.get());
            EXPECT_EQ(std::string("Request ") +  std::to_string(current), *res.data);
            EXPECT_FALSE(bool(res.expires));
            EXPECT_FALSE(bool(res.modified));
            EXPECT_FALSE(bool(res.etag));

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
