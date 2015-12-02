#import <Foundation/Foundation.h>

#import "MGLTypes.h"

NS_ASSUME_NONNULL_BEGIN

void mgl_linkBundleCategory();

@interface NSBundle (MGLAdditions)

/// Returns the Mapbox.framework bundle.
+ (instancetype)mgl_frameworkBundle;

@end

NS_ASSUME_NONNULL_END
