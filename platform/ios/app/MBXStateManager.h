#import <Foundation/Foundation.h>
#import "MBXState.h"

NS_ASSUME_NONNULL_BEGIN

@interface MBXStateManager : NSObject

@property (nonatomic, nullable) MBXState* currentState;

+ (instancetype) sharedManager;

- (void)saveState:(MBXState*)state forKey:(NSObject*)key;

- (void)restoreState;

- (void)resetState;

@end

NS_ASSUME_NONNULL_END
