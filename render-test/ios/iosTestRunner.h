#import <Foundation/Foundation.h>

__attribute__((visibility ("default")))
@interface IosTestRunner : NSObject

- (NSString*) getStyleResultPath;
- (NSString*) getMetricResultPath;
- (NSString*) getMetricPath;
- (BOOL) getTestStatus;

@end
