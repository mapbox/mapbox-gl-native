#import <Foundation/Foundation.h>
#include <mbgl/interface/native_apple_interface.h>

@interface MGLNetworkIntegrationManager : NSObject <MGLNativeNetworkDelegate>

+ (MGLNetworkIntegrationManager *)sharedManager;

@end
