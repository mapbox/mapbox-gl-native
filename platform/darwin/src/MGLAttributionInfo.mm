#import "MGLAttributionInfo.h"

#if TARGET_OS_IPHONE
    #import <UIKit/UIKit.h>
#else
    #import <Cocoa/Cocoa.h>
#endif

#import "MGLMapCamera.h"
#import "NSString+MGLAdditions.h"

#include <string>

@implementation MGLAttributionInfo

+ (NS_ARRAY_OF(MGLAttributionInfo *) *)attributionInfosFromHTMLString:(NSString *)htmlString fontSize:(CGFloat)fontSize linkColor:(nullable MGLColor *)linkColor {
    NSDictionary *options = @{
        NSDocumentTypeDocumentAttribute: NSHTMLTextDocumentType,
        NSCharacterEncodingDocumentAttribute: @(NSUTF8StringEncoding),
    };
    // Apply a bogus, easily detectable style rule to any feedback link, since
    // NSAttributedString doesn’t preserve the class attribute.
    NSMutableString *css = [NSMutableString stringWithString:
                            @".mapbox-improve-map { -webkit-text-stroke-width: 1000px; }"];
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
        [css appendFormat:
         @"a:link { color: rgba(%f%%, %f%%, %f%%, %f); }",
         red * 100, green * 100, blue * 100, alpha];
    }
    NSString *styledHTML = [NSString stringWithFormat:@"<style type='text/css'>%@</style>%@", css, htmlString];
    NSData *htmlData = [styledHTML dataUsingEncoding:NSUTF8StringEncoding];
    
#if TARGET_OS_IPHONE
    NSMutableAttributedString *attributedString = [[NSMutableAttributedString alloc] initWithData:htmlData
                                                                                          options:options
                                                                               documentAttributes:nil
                                                                                            error:NULL];
#else
    NSMutableAttributedString *attributedString = [[NSMutableAttributedString alloc] initWithHTML:htmlData
                                                                                          options:options
                                                                               documentAttributes:nil];
#endif
    
    NSMutableArray *infos = [NSMutableArray array];
    [attributedString enumerateAttribute:NSLinkAttributeName
                                 inRange:attributedString.mgl_wholeRange
                                 options:0
                              usingBlock:
    ^(id _Nullable value, NSRange range, BOOL * _Nonnull stop) {
        NSCAssert(!value || [value isKindOfClass:[NSURL class]], @"If present, URL attribute must be an NSURL.");
        
        // Detect feedback links by the bogus style rule applied above.
        NSNumber *strokeWidth = [attributedString attribute:NSStrokeWidthAttributeName
                                                    atIndex:range.location
                                             effectiveRange:NULL];
        BOOL isFeedbackLink = NO;
        if ([strokeWidth floatValue] > 100) {
            isFeedbackLink = YES;
            [attributedString removeAttribute:NSStrokeWidthAttributeName range:range];
        }
        
        // Omit whitespace-only strings.
        NSAttributedString *title = [[attributedString attributedSubstringFromRange:range]
                                     mgl_attributedStringByTrimmingCharactersInSet:[NSCharacterSet whitespaceAndNewlineCharacterSet]];
        if (!title.length) {
            return;
        }
        
        MGLAttributionInfo *info = [[MGLAttributionInfo alloc] initWithTitle:title URL:value];
        info.feedbackLink = isFeedbackLink;
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

- (nullable NSURL *)feedbackURLAtCenterCoordinate:(CLLocationCoordinate2D)centerCoordinate zoomLevel:(double)zoomLevel {
    if (!self.feedbackLink) {
        return nil;
    }
    
    NSURLComponents *components = [NSURLComponents componentsWithURL:self.URL resolvingAgainstBaseURL:NO];
    components.fragment = [NSString stringWithFormat:@"/%.5f/%.5f/%i",
                           centerCoordinate.longitude, centerCoordinate.latitude, (int)round(zoomLevel + 1)];
    return components.URL;
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
