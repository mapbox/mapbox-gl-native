#import "MGLAttributionInfo.h"
#import "NSString+MGLAdditions.h"

#if TARGET_OS_IPHONE
    #import <UIKit/UIKit.h>
#else
    #import <Cocoa/Cocoa.h>
#endif

#include <string>

/**
 Absolute string of the URL to the Map Feedback tool.
 */
static NSString * const MGLAttributionFeedbackURLString = @"https://www.mapbox.com/map-feedback/";

@implementation MGLAttributionInfo

+ (NS_ARRAY_OF(MGLAttributionInfo *) *)attributionInfosFromHTMLString:(NSString *)htmlString fontSize:(CGFloat)fontSize linkColor:(nullable MGLColor *)linkColor {
    NSDictionary *options = @{
        NSDocumentTypeDocumentAttribute: NSHTMLTextDocumentType,
        NSCharacterEncodingDocumentAttribute: @(NSUTF8StringEncoding),
    };
    NSMutableString *css = [NSMutableString string];
    if (fontSize) {
        [css appendFormat:@"html { font-size: %.1fpx; }", fontSize];
    }
    if (linkColor) {
        CGFloat red;
        CGFloat green;
        CGFloat blue;
        CGFloat alpha;
#if !TARGET_OS_IPHONE
        linkColor = [linkColor colorUsingColorSpaceName:NSCalibratedRGBColorSpace];
#endif
        [linkColor getRed:&red green:&green blue:&blue alpha:&alpha];
        [css appendFormat:@"a:link { color: rgba(%f%%, %f%%, %f%%, %f); }",
         red * 100, green * 100, blue * 100, alpha];
    }
    NSString *styledHTML = [NSString stringWithFormat:@"<style type='text/css'>%@</style>%@", css, htmlString];
    NSData *htmlData = [styledHTML dataUsingEncoding:NSUTF8StringEncoding];
    
#if TARGET_OS_IPHONE
    NSAttributedString *attributedString = [[NSAttributedString alloc] initWithData:htmlData
                                                                            options:options
                                                                 documentAttributes:nil
                                                                              error:NULL];
#else
    NSAttributedString *attributedString = [[NSAttributedString alloc] initWithHTML:htmlData
                                                                            options:options
                                                                 documentAttributes:nil];
#endif
    
    NSMutableArray *infos = [NSMutableArray array];
    [attributedString enumerateAttribute:NSLinkAttributeName
                                 inRange:attributedString.mgl_wholeRange
                                 options:0
                              usingBlock:^(id _Nullable value, NSRange range, BOOL * _Nonnull stop) {
        // Omit the Map Feedback link because the SDK already provides the appropriate UI for giving feedback.
        // Ideally we’d look for class="mapbox-improve-map", but NSAttributedString loses that information.
        NSCAssert(!value || [value isKindOfClass:[NSURL class]], @"If present, URL attribute must be an NSURL.");
        if ([value isEqual:[NSURL URLWithString:MGLAttributionFeedbackURLString]]) {
            return;
        }
        
        // Omit whitespace-only strings.
        NSAttributedString *title = [attributedString attributedSubstringFromRange:range];
        if (![title.string stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceAndNewlineCharacterSet]].length) {
            return;
        }
        
        MGLAttributionInfo *info = [[MGLAttributionInfo alloc] initWithTitle:title URL:value];
        [infos addObject:info];
    }];
    return infos;
}

- (instancetype)initWithTitle:(NSAttributedString *)title URL:(NSURL *)URL {
    if (self = [super init]) {
        _title = title;
        _URL = URL;
    }
    return self;
}

@end
