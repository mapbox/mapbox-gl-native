#import <Foundation/Foundation.h>
@class MBXState;

NS_ASSUME_NONNULL_BEGIN

@interface MBXStateManager : NSObject

+ (instancetype) sharedManager;

- (void)saveState:(MBXState*)state;

- (MBXState *)currentState;

- (void)restoreState;

- (void)resetState;

@end

NS_ASSUME_NONNULL_END
