#import <Foundation/Foundation.h>

#include <mbgl/test/util.hpp>

int main(int argc, char* argv[]) {
    [[NSFileManager defaultManager] changeCurrentDirectoryPath:[[NSBundle mainBundle] bundlePath]];

    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
