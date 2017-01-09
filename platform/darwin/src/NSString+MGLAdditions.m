#import "NSString+MGLAdditions.h"

@implementation NSString (MGLAdditions)

- (NSRange)mgl_wholeRange {
    return NSMakeRange(0, self.length);
}

- (nullable NSString *)mgl_stringOrNilIfEmpty {
    return self.length ? self : nil;
}

- (NSString *)mgl_titleCasedStringWithLocale:(NSLocale *)locale {
    NSMutableString *string = self.mutableCopy;
    [string enumerateLinguisticTagsInRange:string.mgl_wholeRange scheme:NSLinguisticTagSchemeLexicalClass options:0 orthography:nil usingBlock:^(NSString * _Nonnull tag, NSRange tokenRange, NSRange sentenceRange, BOOL * _Nonnull stop) {
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
