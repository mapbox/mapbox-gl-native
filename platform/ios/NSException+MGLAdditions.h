#import <Foundation/Foundation.h>

#define MGLAssertIsMainThread() NSAssert([[NSThread currentThread] isMainThread], @"-[%@ %@] must be accessed on the main thread, not %@", [self class], NSStringFromSelector(_cmd), [NSThread currentThread])
