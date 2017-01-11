#import <Foundation/Foundation.h>

#include <mbgl/util/default_thread_pool.hpp>

@interface MGLThreadPool : NSObject
@end

@interface MGLThreadPool (Private)

/// Returns the shared instance of the `MGLThreadPool` class.
+ (instancetype)sharedPool;

/**
 The shared thread pool object owned by the shared thread pool object.
 */
@property (nonatomic) mbgl::ThreadPool *mbglThreadPool;

@end
