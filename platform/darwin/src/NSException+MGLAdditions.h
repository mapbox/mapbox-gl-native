#import <Foundation/Foundation.h>
#import "MGLLoggingConfiguration_Private.h"

#define MGLAssertIsMainThread() MGLAssert([[NSThread currentThread] isMainThread], @"%s must be accessed on the main thread, not %@", __PRETTY_FUNCTION__, [NSThread currentThread])
