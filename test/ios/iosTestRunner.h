#import <Foundation/Foundation.h>

NSString *dataDir = @"test-data";
NSString *resultFilePath = @"test/results.xml";

__attribute__((visibility ("default")))
@interface IosTestRunner : NSObject

- (NSString*) getResultPath;
- (BOOL) getTestStatus;

@end
