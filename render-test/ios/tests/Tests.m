#import <XCTest/XCTest.h>
#import "iosTestRunner.h"
@interface Tests : XCTestCase

@end

@implementation Tests

- (void)setUp {
    // Put setup code here. This method is called before the invocation of each test method in the class.

    // In UI tests it is usually best to stop immediately when a failure occurs.
    [super setUp];

    // In UI tests it’s important to set the initial state - such as interface orientation - required for your tests before they run. The setUp method is a good place to do this.
}

- (void)tearDown {
    // Put teardown code here. This method is called after the invocation of each test method in the class.
    [super tearDown];
}

- (void)testExample {
    // UI tests must launch the application that they test.
    IosTestRunner* runner = [[IosTestRunner alloc] init]; 
    XCTAssert(runner);
    XCTAttachment *attachment = [XCTAttachment attachmentWithString:@"Hello"];
    XCTAssert(attachment);
    attachment.lifetime = XCTAttachmentLifetimeKeepAlways;
    [self addAttachment:attachment];
    
    NSString* result = [runner getResultPath];
    XCTAssert(result);
    NSFileManager *fileManager = [NSFileManager defaultManager];
    BOOL success = [fileManager fileExistsAtPath: result];
    if (!success) {
       NSLog(@"File doese not exit %@", result);
    }
    NSURL *url = [NSURL fileURLWithPath:result];
    
    XCTAttachment *attachmentURL = [XCTAttachment attachmentWithContentsOfFileAtURL: url];
    XCTAssert(attachmentURL);
    attachmentURL.lifetime = XCTAttachmentLifetimeKeepAlways;
    [self addAttachment:attachmentURL];

    // Use recording to get started writing UI tests.
    // Use XCTAssert and related functions to verify your tests produce the correct results.
}

@end
