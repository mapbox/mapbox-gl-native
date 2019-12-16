#import "iosTestRunner.h"

#import "ios_test_runner.hpp"

#include <string>

@interface IosTestRunner ()
{
    TestRunner* runner;
}
@end

@implementation IosTestRunner

-(instancetype)init
{
    self = [super init];
    if (self) {
        runner = new TestRunner();
        NSString *path = [[NSBundle mainBundle] pathForResource:@"mac-manifest" ofType:@"json"];
        
//        NSString * path1 = [[NSBundle mainBundle] pathForResource:@"package-lock" ofType:@"json" inDirectory:@"integration"];
        NSString *path1 = [[NSBundle mainBundle] bundlePath];
        (void)path1;
//        NSURL *url = [NSURL fileURLWithPath:path];
        std::string manifest = std::string([path UTF8String]);
        runner->startTest(manifest);
        delete runner;
        runner = nullptr;
    }
    return self;
}

@end
