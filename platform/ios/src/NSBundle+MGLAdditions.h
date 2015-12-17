#import <Foundation/Foundation.h>

#import "MGLTypes.h"

NS_ASSUME_NONNULL_BEGIN

void mgl_linkBundleCategory();

@interface NSBundle (MGLAdditions)

+ (NSString *)mgl_resourceBundlePath;

@end

NS_ASSUME_NONNULL_END
