#import "NSString+MGLAdditions.h"

@implementation NSString (MGLAdditions)

- (NSRange)mgl_wholeRange {
    return NSMakeRange(0, self.length);
}

- (nullable NSString *)mgl_stringOrNilIfEmpty {
    return self.length ? self : nil;
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
