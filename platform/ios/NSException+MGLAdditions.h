#import <Foundation/Foundation.h>

#define MGLAssertIsMainThread() NSAssert([[NSThread currentThread] isMainThread], @"%s must be accessed on the main thread, not %@", __PRETTY_FUNCTION__, [NSThread currentThread])
