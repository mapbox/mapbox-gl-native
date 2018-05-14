#import "MGLVectorTileSource.h"

NS_ASSUME_NONNULL_BEGIN

@interface MGLVectorTileSource (Private)

@property (nonatomic, readonly, getter=isMapboxStreets) BOOL mapboxStreets;

+ (NS_SET_OF(NSString *) *)mapboxStreetsLanguages;

+ (nullable NSString *)preferredMapboxStreetsLanguage;
+ (nullable NSString *)preferredMapboxStreetsLanguageForPreferences:(NSArray<NSString *> *)preferencesArray;

@end

NS_ASSUME_NONNULL_END
