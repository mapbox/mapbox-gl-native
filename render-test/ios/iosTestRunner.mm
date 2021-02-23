#import "iosTestRunner.h"

#include "ios_test_runner.hpp"

#import "SSZipArchive.h"

#include <string>

@interface IosTestRunner ()

@property (nullable) TestRunner* runner;

@property (copy, nullable) NSString *styleResultPath;

@property (copy, nullable) NSString *metricResultPath;

@property (copy, nullable) NSString *metricPath;

@property BOOL testStatus;

@end

@implementation IosTestRunner

-(instancetype)init
{
    self = [super init];
    if (self) {
        self.testStatus = NO;
        self.runner = new TestRunner();
        NSString *path = nil;
        NSError *error;
        NSFileManager *fileManager = [NSFileManager defaultManager];
        NSString *bundleRoot = [[NSBundle mainBundle] bundlePath];
        NSArray *bundleContents = [fileManager contentsOfDirectoryAtPath: bundleRoot error: &error];
        NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
        NSString *documentsDir = [paths objectAtIndex: 0];
        
        for (uint32_t i = 0; i < bundleContents.count; i++) {
            NSString *dirName = [bundleContents objectAtIndex: i];
            if ([dirName isEqualToString:@"test-data"]) {
                NSString *destinationPath = [documentsDir stringByAppendingPathComponent: dirName];
                //Using NSFileManager we can perform many file system operations.
                BOOL success = [fileManager fileExistsAtPath: destinationPath];
                if (success) {
                    success = [fileManager removeItemAtPath:destinationPath error:NULL];
                }
                break;
            }
        }
        
        for (uint32_t i = 0; i < bundleContents.count; i++) {
            NSString *dirName = [bundleContents objectAtIndex: i];
            if ([dirName isEqualToString:@"test-data"]) {
                NSString *destinationPath = [documentsDir stringByAppendingPathComponent: dirName];
                BOOL success = [fileManager fileExistsAtPath: destinationPath];
                if (!success) {
                    NSString *copyDirPath  = [[[NSBundle mainBundle] resourcePath] stringByAppendingPathComponent: dirName];

                    success = [fileManager copyItemAtPath: copyDirPath toPath: destinationPath error: &error];
              
                    if (!success){
                        NSAssert1(0, @"Failed to copy file '%@'.", [error localizedDescription]);
                        NSLog(@"Failed to copy %@ file, error %@", dirName, [error localizedDescription]);
                    }
                    else {
                        path = destinationPath;
                        NSLog(@"File copied %@ OK", dirName);
                    }
                }
                else {
                    NSLog(@"File exits %@, skip copy", dirName);
                }
                break;
            }
        }
        if (path) {

            std::string basePath = std::string([path UTF8String]);
            self.testStatus = self.runner->startTest(basePath) ? YES : NO;
            self.styleResultPath =  [path stringByAppendingPathComponent:@"/ios-render-test-runner-style.html"];
            self.metricResultPath =  [path stringByAppendingPathComponent:@"/ios-render-test-runner-metrics.html"];

            BOOL fileFound = [fileManager fileExistsAtPath: self.styleResultPath];
            if (fileFound == NO) {
                NSLog(@"Style test result file '%@' doese not exit ", self.styleResultPath);
                self.testStatus = NO;
            }

            fileFound = [fileManager fileExistsAtPath: self.metricResultPath];
            if (fileFound == NO) {
                NSLog(@"Metric test result file '%@' doese not exit ", self.metricResultPath);
                self.testStatus = NO;
            }

            NSString *rebaselinePath = [path stringByAppendingPathComponent:@"/baselines"];
            BOOL needArchiving = NO;
            BOOL isDir = NO;
            NSArray *subpaths = [[NSArray alloc] init];
            if ([fileManager fileExistsAtPath:rebaselinePath isDirectory: &isDir] && isDir){
                subpaths = [fileManager subpathsAtPath:rebaselinePath];
                for(NSString *path in subpaths)
                {
                    NSString *longPath = [rebaselinePath stringByAppendingPathComponent:path];
                    if([fileManager fileExistsAtPath:longPath isDirectory:&isDir] && !isDir)
                    {
                        needArchiving = YES;
                        break;
                    }
                }
            }
            else {
               NSLog(@"Metric path '%@' doese not exit ", rebaselinePath);
            }

            if (needArchiving) {
                NSString *archivePath = [path stringByAppendingString:@"/metrics.zip"];
                BOOL success = [SSZipArchive createZipFileAtPath:archivePath
                withContentsOfDirectory:rebaselinePath
                    keepParentDirectory:NO
                       compressionLevel:-1
                               password:nil
                                    AES:YES
                        progressHandler:nil];

                if (success) {
                    NSLog(@"Successfully archive all of the metrics into metrics.zip");
                    self.metricPath =  archivePath;
                }
                else {
                    NSLog(@"Failed to archive rebaselined metrics into metrics.zip");
                }
            }
        }

        delete self.runner;
        self.runner = nullptr;
    }
    return self;
}

- (NSString*) getStyleResultPath {
    return self.styleResultPath;
}

- (NSString*) getMetricResultPath {
    return self.metricResultPath;
}

- (NSString*) getMetricPath {
    return self.metricPath;
}

- (BOOL) getTestStatus {
    return self.testStatus;
}
@end
