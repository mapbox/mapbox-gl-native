#import <UIKit/UIKit.h>

@interface UIColor (MGLAdditions)

+ (UIColor *)colorWithRGBAString:(NSString *)rgbaString;
- (NSString *)rgbaStringFromColor;

+ (UIColor *)colorWithHexString:(NSString *)hexString;
- (NSString *)hexStringFromColor;

@end
