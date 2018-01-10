#import "MGLVectorSource.h"

NS_ASSUME_NONNULL_BEGIN

@interface MGLVectorSource (Private)

@property (nonatomic, readonly, getter=isMapboxStreets) BOOL mapboxStreets;

+ (NS_SET_OF(NSString *) *)mapboxStreetsLanguages;

+ (NSString *)preferredMapboxStreetsLanguage;

- (NS_DICTIONARY_OF(NSString *, NSString *) *)localizedKeysByKeyForPreferredLanguage:(nullable NSString *)preferredLanguage;

@end

NS_ASSUME_NONNULL_END
