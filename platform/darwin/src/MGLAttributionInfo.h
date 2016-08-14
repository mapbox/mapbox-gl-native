#import <Foundation/Foundation.h>
#import <CoreGraphics/CoreGraphics.h>

#import "MGLTypes.h"

#include <vector>

NS_ASSUME_NONNULL_BEGIN

/**
 Information about an attribution statement, usually a copyright or trademark
 statement, associated with a map source.
 */
@interface MGLAttributionInfo : NSObject

/**
 Parses and returns the attribution infos contained in the given HTML source
 code string.
 
 @param htmlString The HTML source code to parse.
 @param fontSize The default text size in points.
 @param linkColor The default link color.
 */
+ (NS_ARRAY_OF(MGLAttributionInfo *) *)attributionInfosFromHTMLString:(NSString *)htmlString fontSize:(CGFloat)fontSize linkColor:(nullable MGLColor *)linkColor;

- (instancetype)initWithTitle:(NSAttributedString *)title URL:(NSURL *)URL;

@property (nonatomic) NSAttributedString *title;
@property (nonatomic, nullable) NSURL *URL;

@end

NS_ASSUME_NONNULL_END
