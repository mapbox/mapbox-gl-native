#import <Foundation/Foundation.h>

@interface NSOrthography (NSOrthography_MGLAdditions)

/**
 Returns a four-letter ISO 15924 code representing the name of the dominant
 script for a given language.

 On iOS 11 or newer, this method wraps
 `+[NSOrthography defaultOrthographyForLanguage:]` and supports any language.
 On iOS 10 and older, this method only returns values for Mapbox
 Streets-supported languages.

 @param language The ISO-639 code representing a language.
 */
+ (NSString *)mgl_dominantScriptForMapboxStreetsLanguage:(NSString *)language;

@end
