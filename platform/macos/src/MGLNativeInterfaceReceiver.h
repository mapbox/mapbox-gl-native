#import <Foundation/Foundation.h>
#include <mbgl/interface/native_apple_interface.h>

@interface MGLNativeInterfaceReceiver : NSObject <MGLNativeAppleInterfaceDelegate>

+ (MGLNativeInterfaceReceiver *)shared;

@end
