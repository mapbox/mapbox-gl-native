#import <Foundation/Foundation.h>

NSString *dataDir = @"benchmark-data";
NSString *resultFilePath = @"benchmark/results.json";

__attribute__((visibility ("default")))
@interface IosTestRunner : NSObject

- (NSString*) getResultPath;
- (BOOL) getTestStatus;

@end
