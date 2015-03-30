#import <Foundation/Foundation.h>

@interface NSString (MGLAdditions)

/// Returns the receiver if non-empty or \c nil if empty.
- (NSString *)mgl_stringOrNilIfEmpty;

@end
