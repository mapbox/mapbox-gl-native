#import <Foundation/Foundation.h>

__attribute__((visibility ("default")))
@interface IosTestRunner : NSObject

- (NSString*) getResultPath;
- (BOOL) getTestStatus;

@end
