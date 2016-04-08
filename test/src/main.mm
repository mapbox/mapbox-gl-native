#include <mbgl/test.hpp>

#import <Foundation/Foundation.h>

int main(int argc, char* argv[]) {
    [[NSFileManager defaultManager] changeCurrentDirectoryPath:[[NSBundle mainBundle] bundlePath]];
    return mbgl::runTests(argc, argv);
}
