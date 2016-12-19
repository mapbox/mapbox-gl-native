#import "MGLOfflineStorage.h"

NS_ASSUME_NONNULL_BEGIN

@interface MGLOfflineStorage (MGLURLSchemeTemplateAdditions)

- (void)setURLTemplate:(nullable NSString *)tpl forScheme:(NSString *)scheme;

@end

NS_ASSUME_NONNULL_END
