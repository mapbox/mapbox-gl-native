#import <XCTest/XCTest.h>
#import "iosTestRunner.h"
@interface Tests : XCTestCase

@end

@implementation Tests

- (void)setUp {
    [super setUp];
}

- (void)tearDown {
    [super tearDown];
}

- (void)testStartRenderTestRunner {
    IosTestRunner* runner = [[IosTestRunner alloc] init]; 
    XCTAssert(runner, @"IOSTestRunner is not initialized correctly");
    
    NSString* result = [runner getResultPath];
    XCTAssert(result, @"IOSTestRunner does not produce a result file");

    NSFileManager *fileManager = [NSFileManager defaultManager];
    BOOL success = [fileManager fileExistsAtPath: result];
    XCTAssert(success, @"Test result html '%@' doese not exit", result);

    NSURL *url = [NSURL fileURLWithPath:result]; 
    XCTAttachment *attachmentURL = [XCTAttachment attachmentWithContentsOfFileAtURL: url];
    XCTAssert(attachmentURL, @"Failed to attach test result '%@'", result);
    attachmentURL.lifetime = XCTAttachmentLifetimeKeepAlways;
    [self addAttachment:attachmentURL];
}

@end
