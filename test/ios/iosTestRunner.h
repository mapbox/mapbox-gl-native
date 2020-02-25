#import <Foundation/Foundation.h>

NSString *dataDir = @"test-data";
NSString *resultFilePath = @"test/fixtures/annotations/debug_sparse/actual.png";

__attribute__((visibility ("default")))
@interface IosTestRunner : NSObject

- (NSString*) getResultPath;
- (BOOL) getTestStatus;

@end
