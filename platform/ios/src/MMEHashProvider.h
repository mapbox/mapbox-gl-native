#import <Foundation/Foundation.h>
#import "MMEEventsConfiguration.h"

@interface MMEHashProvider : NSObject

@property (nonatomic, readonly) NSArray *cnHashes;
@property (nonatomic, readonly) NSArray *comHashes;

- (void)updateHashesWithConfiguration:(MMEEventsConfiguration *)configuration;

@end
