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

- (BOOL)isEqual:(id)object {
    return [object isKindOfClass:[self class]] && [[object title] isEqual:self.title] && [[object URL] isEqual:self.URL];
}

- (NSUInteger)hash {
    return self.title.hash + self.URL.hash;
}

/**
 Returns whether the given attribution info object overlaps with the receiver by
 its plain text title.
 
 @return `NSOrderedAscending` if the given object is a superset of the receiver,
    `NSOrderedDescending` if it is a subset of the receiver, or `NSOrderedSame`
    if there is no overlap.
 */
- (NSComparisonResult)subsetCompare:(MGLAttributionInfo *)otherInfo {
    NSString *title = self.title.string;
    NSString *otherTitle = otherInfo.title.string;
    if ([title containsString:otherTitle]) {
        return NSOrderedDescending;
    }
    if ([otherTitle containsString:title]) {
        return NSOrderedAscending;
    }
    return NSOrderedSame;
}

@end

@implementation NSMutableArray (MGLAttributionInfoAdditions)

- (void)growArrayByAddingAttributionInfo:(MGLAttributionInfo *)info {
    __block BOOL didInsertInfo = NO;
    __block BOOL shouldAddInfo = YES;
    [self enumerateObjectsUsingBlock:^(MGLAttributionInfo * _Nonnull existingInfo, NSUInteger idx, BOOL * _Nonnull stop) {
        switch ([info subsetCompare:existingInfo]) {
            case NSOrderedDescending:
                // The existing info object is a subset of the one we’re adding.
                // Replace the existing object the first time we find a subset;
                // remove the existing object every time after that.
                if (didInsertInfo) {
                    [self removeObjectAtIndex:idx];
                } else {
                    [self replaceObjectAtIndex:idx withObject:info];
                    didInsertInfo = YES;
                }
                break;
                
            case NSOrderedAscending:
                // The info object we’re adding is a subset of the existing one.
                // Don’t add the object and stop looking.
                shouldAddInfo = NO;
                *stop = YES;
                break;
                
            default:
                break;
        }
    }];
    if (shouldAddInfo && !didInsertInfo) {
        // No overlapping infos were found, so append the info object.
        [self addObject:info];
    }
}

- (void)growArrayByAddingAttributionInfosFromArray:(NS_ARRAY_OF(MGLAttributionInfo *) *)infos {
    for (MGLAttributionInfo *info in infos) {
        [self growArrayByAddingAttributionInfo:info];
    }
}

@end
