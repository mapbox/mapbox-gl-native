#import "MGLOfflineTask.h"

#include <mbgl/storage/default_file_source.hpp>

@interface MGLOfflineTask (Private)

@property (nonatomic) mbgl::OfflineRegion *mbglOfflineRegion;

- (instancetype)initWithMBGLRegion:(mbgl::OfflineRegion *)region;

- (void)invalidate;

@end
