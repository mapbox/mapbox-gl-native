#import "NSOrthography+MGLAdditions.h"

@implementation NSOrthography (MGLAdditions)

+ (NSString *)mgl_dominantScriptForMapboxStreetsLanguage:(NSString *)language {
    if (@available(iOS 11.0, *)) {
        NSLocale *locale = [NSLocale localeWithLocaleIdentifier:language];
        NSOrthography *orthography = [NSOrthography defaultOrthographyForLanguage:locale.localeIdentifier];

        return orthography.dominantScript;
    }

    // Manually map Mapbox Streets languages to ISO 15924 script codes.
    NSSet *latinLanguages = [NSSet setWithObjects:@"de", @"en", @"es", @"fr", @"pt", nil];
    NSSet *hansLanguages = [NSSet setWithObjects:@"zh", @"zh-Hans", nil];

    if ([latinLanguages containsObject:language]) {
        return @"Latn";
    } else if ([hansLanguages containsObject:language]) {
        return @"Hans";
    } else if ([language isEqualToString:@"ru"]) {
        return @"Cyrl";
    } else if ([language isEqualToString:@"ar"]) {
        return @"Arab";
    } else {
        // Code for undetermined script
        return @"Zyyy";
    }
}

@end
