#include <mbgl/test.hpp>
#include <mbgl/test/util.hpp>

#include <gtest/gtest.h>

namespace mbgl {

int runTests(int argc, char *argv[]) {
    auto server = std::make_unique<test::Server>("test/storage/server.js");
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

}
