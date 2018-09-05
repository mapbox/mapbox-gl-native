#import <Foundation/Foundation.h>
#import "MMEEventsConfiguration.h"

@class TrustKit;

@interface MMETrustKitProvider : NSObject

+ (TrustKit *)trustKitWithUpdatedConfiguration:(MMEEventsConfiguration *)configuration;

@end
