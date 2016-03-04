#import "MGLDownloadController.h"

#import "MGLDownloadable.h"

#include <mbgl/storage/default_file_source.hpp>

NS_ASSUME_NONNULL_BEGIN

@interface MGLDownloadController (Private)

@property (nonatomic) mbgl::DefaultFileSource *mbglFileSource;

@end

NS_ASSUME_NONNULL_END
