#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface NSString (MGLAdditions)

/** Returns the range spanning the entire receiver. */
- (NSRange)mgl_wholeRange;

/** Returns the receiver if non-empty or nil if empty. */
- (nullable NSString *)mgl_stringOrNilIfEmpty;

@end

@interface NSAttributedString (MGLAdditions)

/** Returns the range spanning the entire receiver. */
- (NSRange)mgl_wholeRange;

/** Returns a copy of the receiver with leading and trailing members of the given set removed. */
- (NSAttributedString *)mgl_attributedStringByTrimmingCharactersInSet:(NSCharacterSet *)set;

@end

NS_ASSUME_NONNULL_END
