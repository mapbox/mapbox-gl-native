#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface NSString (MGLAdditions)

/** Returns the range spanning the entire receiver. */
- (NSRange)mgl_wholeRange;

/** Returns the receiver if non-empty or nil if empty. */
- (nullable NSString *)mgl_stringOrNilIfEmpty;

/**
 Returns a title-cased representation of the receiver using the specified
 locale.

 @param locale The locale. For strings presented to users, pass in the current
    locale (`+[NSLocale currentLocale]`). To use the system locale, pass in
    `nil`.
 */
- (NSString *)mgl_titleCasedStringWithLocale:(NSLocale *)locale;

/**
 Returns a transliterated representation of the receiver using the specified
 script. If transliteration fails, the receiver will be returned.

 Only supports scripts for languages used by Mapbox Streets.

 On iOS 8 or older, this will method will always return the untransliterated
 receiver.

 @param script The four-letter code representing the name of the script, as
    specified by ISO 15924.
 */
- (NSString *)mgl_stringByTransliteratingIntoScript:(NSString *)script;

@end

@interface NSAttributedString (MGLAdditions)

/** Returns the range spanning the entire receiver. */
- (NSRange)mgl_wholeRange;

/** Returns a copy of the receiver with leading and trailing members of the given set removed. */
- (NSAttributedString *)mgl_attributedStringByTrimmingCharactersInSet:(NSCharacterSet *)set;

@end

NS_ASSUME_NONNULL_END
