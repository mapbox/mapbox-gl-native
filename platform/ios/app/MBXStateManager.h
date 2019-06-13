#import <Foundation/Foundation.h>
#import "MBXViewController.h"
@class MBXState;

NS_ASSUME_NONNULL_BEGIN

@interface MBXStateManager : NSObject

+ (instancetype) sharedManager;

- (MBXState *)currentState;

- (void)saveState:(MBXState*)mapViewController;

- (void)resetState;

@end

NS_ASSUME_NONNULL_END
