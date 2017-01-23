#import "MGLVectorSource+MBXAdditions.h"

@implementation MGLVectorSource (MBXAdditions)

+ (NS_SET_OF(NSString *) *)mapboxStreetsLanguages {
    // https://www.mapbox.com/vector-tiles/mapbox-streets-v7/#overview
    static dispatch_once_t onceToken;
    static NS_SET_OF(NSString *) *mapboxStreetsLanguages;
    dispatch_once(&onceToken, ^{
        mapboxStreetsLanguages = [NSSet setWithObjects:@"en", @"es", @"fr", @"de", @"ru", @"zh", nil];
    });
    return mapboxStreetsLanguages;
}

+ (nullable NSString *)preferredMapboxStreetsLanguage {
    for (NSString *language in [NSLocale preferredLanguages]) {
        NSString *languageCode = [[NSLocale localeWithLocaleIdentifier:language] objectForKey:NSLocaleLanguageCode];
        if ([[MGLVectorSource mapboxStreetsLanguages] containsObject:languageCode]) {
            return languageCode;
        }
    }
    return nil;
}

- (BOOL)isMapboxStreets {
    NSURL *url = self.configurationURL;
    if (![url.scheme isEqualToString:@"mapbox"]) {
        return NO;
    }
    NSArray *identifiers = [url.host componentsSeparatedByString:@","];
    return [identifiers containsObject:@"mapbox.mapbox-streets-v7"] || [identifiers containsObject:@"mapbox.mapbox-streets-v6"];
}

- (NS_DICTIONARY_OF(NSString *, NSString *) *)localizedKeysByKeyForPreferredLanguage:(nullable NSString *)preferredLanguage {
    if (!self.mapboxStreets) {
        return @{};
    }
    
    // Replace {name} and {name_*} with the matching localized name tag.
    NSString *localizedKey = preferredLanguage ? [NSString stringWithFormat:@"name_%@", preferredLanguage] : @"name";
    NSMutableDictionary *localizedKeysByKey = [NSMutableDictionary dictionaryWithObject:localizedKey forKey:@"name"];
    for (NSString *languageCode in [MGLVectorSource mapboxStreetsLanguages]) {
        NSString *key = [NSString stringWithFormat:@"name_%@", languageCode];
        localizedKeysByKey[key] = localizedKey;
    }
    return localizedKeysByKey;
}

@end
