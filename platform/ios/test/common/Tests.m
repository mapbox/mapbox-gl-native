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

- (void)testStartTestRunner {
    IosTestRunner* runner = [[IosTestRunner alloc] init];
    XCTAssert(runner, @"IOSTestRunner is not initialized correctly");

    NSString* testResult = [runner getResultPath];
    XCTAssert(testResult, @"IOSTestRunner did not produce a test result file");

    NSFileManager *fileManager = [NSFileManager defaultManager];
    BOOL fileFound = [fileManager fileExistsAtPath: testResult];
    XCTAssert(fileFound, @"Test result file '%@' does not exit", testResult);
    NSURL *resultURL = [NSURL fileURLWithPath:testResult];
    XCTAttachment *attachment1URL = [XCTAttachment attachmentWithContentsOfFileAtURL: resultURL];
    XCTAssert(attachment1URL, @"Failed to attach test result '%@'", testResult);
    attachment1URL.lifetime = XCTAttachmentLifetimeKeepAlways;
    [self addAttachment:attachment1URL];

    BOOL success = [runner getTestStatus];
    XCTAssert(success, @"IOSTestRunner reports error because some of the tests are not passed, please check the test report");
}

@end
