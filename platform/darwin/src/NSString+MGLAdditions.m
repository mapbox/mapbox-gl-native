#import "NSString+MGLAdditions.h"

#if TARGET_OS_OSX
    #import <Availability.h>
#endif

@implementation NSString (MGLAdditions)

- (NSRange)mgl_wholeRange {
    return NSMakeRange(0, self.length);
}

- (nullable NSString *)mgl_stringOrNilIfEmpty {
    return self.length ? self : nil;
}

- (NSString *)mgl_titleCasedStringWithLocale:(NSLocale *)locale {
    NSMutableString *string = self.mutableCopy;
    NSOrthography *orthography;
    if (@available(iOS 11.0, macOS 10.13.0, *)) {
        orthography = [NSOrthography defaultOrthographyForLanguage:locale.localeIdentifier];
    }
    [string enumerateLinguisticTagsInRange:string.mgl_wholeRange scheme:NSLinguisticTagSchemeLexicalClass options:0 orthography:orthography usingBlock:^(NSString * _Nonnull tag, NSRange tokenRange, NSRange sentenceRange, BOOL * _Nonnull stop) {
        NSString *word = [string substringWithRange:tokenRange];
        if (word.length > 3
            || !([tag isEqualToString:NSLinguisticTagConjunction]
                 || [tag isEqualToString:NSLinguisticTagPreposition]
                 || [tag isEqualToString:NSLinguisticTagDeterminer]
                 || [tag isEqualToString:NSLinguisticTagParticle]
                 || [tag isEqualToString:NSLinguisticTagClassifier])) {
            unichar firstLetter = [[word capitalizedStringWithLocale:locale] characterAtIndex:0];
            NSString *suffix = [word substringFromIndex:1];
            if (!([word hasPrefix:@"i"] && suffix.length
                  && [[NSCharacterSet uppercaseLetterCharacterSet] characterIsMember:[suffix characterAtIndex:0]])) {
                word = [NSString stringWithFormat:@"%C%@", firstLetter, suffix];
            }
        }
        [string replaceCharactersInRange:tokenRange withString:word];
    }];
    return string;
}

- (NSString *)mgl_stringByTransliteratingIntoScript:(NSString *)script {
    NSMutableString *string = self.mutableCopy;
    NSStringTransform transform;
    if ([script isEqualToString:@"Latn"]) {
        transform = NSStringTransformToLatin;
    } else if ([script isEqualToString:@"Hans"]) {
        transform = @"Hant-Hans";
    } else if ([script isEqualToString:@"Hant"]) {
        transform = @"Hans-Hant";
    } else if ([script isEqualToString:@"Cyrl"]) {
        transform = @"Any-Latin; Latin-Cyrillic";
    } else if ([script isEqualToString:@"Arab"]) {
        transform = @"Any-Latin; Latin-Arabic";
    } else if ([script isEqualToString:@"Jpan"]) {
        transform = @"Any-Latin; Latin-Katakana";
    } else if ([script isEqualToString:@"Kore"]) {
        transform = @"Any-Latin; Latin-Hangul";
    }
    return transform ? [string stringByApplyingTransform:transform reverse:NO] : string;
}

@end

@implementation NSAttributedString (MGLAdditions)

- (NSRange)mgl_wholeRange {
    return NSMakeRange(0, self.length);
}

- (NSAttributedString *)mgl_attributedStringByTrimmingCharactersInSet:(NSCharacterSet *)set {
    NSScanner *scanner = [NSScanner scannerWithString:self.string];
    scanner.charactersToBeSkipped = nil;
    NSString *prefix;
    [scanner scanCharactersFromSet:set intoString:&prefix];

    NSString *trimmedString = [self.string stringByTrimmingCharactersInSet:set];
    return [self attributedSubstringFromRange:NSMakeRange(prefix.length, trimmedString.length)];
}

@end
