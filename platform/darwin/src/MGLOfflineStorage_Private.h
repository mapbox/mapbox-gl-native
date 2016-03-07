#import "MGLOfflineStorage.h"

#import "MGLOfflineTask.h"

#include <mbgl/storage/default_file_source.hpp>

NS_ASSUME_NONNULL_BEGIN

@interface MGLOfflineStorage (Private)

@property (nonatomic) mbgl::DefaultFileSource *mbglFileSource;

@end

NS_ASSUME_NONNULL_END
