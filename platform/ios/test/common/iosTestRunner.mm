#import "iosTestRunner.h"

#include <ios_test_runner.hpp>

#include <string>

@interface IosTestRunner ()

@property (nullable) TestRunner *runner;

@property (copy, nullable) NSString *resultPath;

@property BOOL testStatus;

@end

@implementation IosTestRunner

-(instancetype)init
{
    self = [super init];
    if (self) {
        self.testStatus = NO;
        self.runner = new TestRunner();
        NSError *error;
        NSFileManager *fileManager = [NSFileManager defaultManager];
        NSString *bundleRoot = [[NSBundle mainBundle] bundlePath];
        NSArray *bundleContents = [fileManager contentsOfDirectoryAtPath: bundleRoot error: &error];
        NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
        NSString *documentsDir = [paths objectAtIndex: 0];
        
        for (uint32_t i = 0; i < bundleContents.count; i++) {
            NSString *dirName = [bundleContents objectAtIndex: i];
            if ([dirName isEqualToString: dataDir]) {
                NSString *destinationPath = [documentsDir stringByAppendingPathComponent: dirName];
                BOOL success = [fileManager fileExistsAtPath: destinationPath];
                if (success) {
                    [fileManager removeItemAtPath:destinationPath error:NULL];
                }

                success = [fileManager fileExistsAtPath: destinationPath];
                if (!success) {
                    NSString *copyDirPath  = [[[NSBundle mainBundle] resourcePath] stringByAppendingPathComponent: dirName];
                    success = [fileManager copyItemAtPath: copyDirPath toPath: destinationPath error: &error];
              
                    if (!success) {
                        NSLog(@"Failed to copy file '%@'", dirName);
                        NSAssert1(0, @"Failed to copy file, error '%@'", [error localizedDescription]);
                    } else {
                        NSLog(@"File '%@' copied OK", dirName);
                    }
                } else {
                    NSLog(@"Failed to remove file '%@'", dirName);
                    NSAssert1(0, @"Failed to remove file, error '%@'", [error localizedDescription]);
                }
                break;
            }
        }
        NSString *baseDir = [documentsDir stringByAppendingPathComponent: dataDir];
        std::string basePath = std::string([baseDir UTF8String]);
        self.testStatus = self.runner->startTest(basePath) ? YES : NO;
        self.resultPath = [baseDir stringByAppendingPathComponent: resultFilePath];

        BOOL fileFound = [fileManager fileExistsAtPath: self.resultPath];
        if (fileFound == NO) {
            NSLog(@"Test result file '%@' does not exist", self.resultPath);
            self.testStatus = NO;
        }

        delete self.runner;
        self.runner = nullptr;
    }
    return self;
}

- (NSString*) getResultPath {
    return self.resultPath;
}

- (BOOL) getTestStatus {
    return self.testStatus;
}
@end
