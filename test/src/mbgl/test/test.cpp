#include <mbgl/actor/scheduler.hpp>
#include <mbgl/test.hpp>
#include <mbgl/test/util.hpp>

#include <gtest/gtest.h>

namespace mbgl {

int runTests(int argc, char *argv[]) {
#if TEST_HAS_SERVER
    auto server = std::make_unique<test::HttpServer>();
#endif

    testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}

} // namespace mbgl
