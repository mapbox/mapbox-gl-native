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

    NSString* styleResult = [runner getStyleResultPath];
    XCTAssert(styleResult, @"IOSTestRunner does not produce a style test result file");
    NSString* metricResult = [runner getMetricResultPath];
    XCTAssert(metricResult, @"IOSTestRunner does not produce a metric test result file");
    
    NSFileManager *fileManager = [NSFileManager defaultManager];
    BOOL fileFound = [fileManager fileExistsAtPath: styleResult];
    XCTAssert(fileFound, @"Test result html '%@' doese not exit", styleResult);
    NSURL *styleURL = [NSURL fileURLWithPath:styleResult];
    XCTAttachment *attachment1URL = [XCTAttachment attachmentWithContentsOfFileAtURL: styleURL];
    XCTAssert(attachment1URL, @"Failed to attach test result '%@'", styleResult);
    attachment1URL.lifetime = XCTAttachmentLifetimeKeepAlways;
    [self addAttachment:attachment1URL];
    
    fileFound = [fileManager fileExistsAtPath: metricResult];
    XCTAssert(fileFound, @"Test result html '%@' doese not exit", metricResult);
    NSURL *metricURL = [NSURL fileURLWithPath:metricResult];
    XCTAttachment *attachment2URL = [XCTAttachment attachmentWithContentsOfFileAtURL: metricURL];
    XCTAssert(attachment2URL, @"Failed to attach test result '%@'", metricResult);
    attachment2URL.lifetime = XCTAttachmentLifetimeKeepAlways;
    [self addAttachment:attachment2URL];

    BOOL success = [runner getTestStatus];
    XCTAssert(success, @"IOSTestRunner reports error because some of the tests are not passed, please check the test report");
}

@end
