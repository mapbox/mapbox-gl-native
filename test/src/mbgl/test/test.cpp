#include <mbgl/test.hpp>
#include <gtest/gtest.h>

namespace mbgl {

int runTests(int argc, char *argv[]) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

}
