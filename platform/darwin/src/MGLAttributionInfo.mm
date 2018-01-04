#import "MGLAttributionInfo_Private.h"

#if TARGET_OS_IPHONE
    #import <UIKit/UIKit.h>
#else
    #import <Cocoa/Cocoa.h>
#endif

#import "MGLAccountManager.h"
#import "MGLMapCamera.h"
#import "NSArray+MGLAdditions.h"
#import "NSBundle+MGLAdditions.h"
#import "NSString+MGLAdditions.h"

#include <string>

@implementation MGLAttributionInfo

+ (NS_ARRAY_OF(MGLAttributionInfo *) *)attributionInfosFromHTMLString:(nullable NSString *)htmlString fontSize:(CGFloat)fontSize linkColor:(nullable MGLColor *)linkColor {
    if (!htmlString) {
        return @[];
    }

    NSDictionary *options = @{
        NSDocumentTypeDocumentAttribute: NSHTMLTextDocumentType,
        NSCharacterEncodingDocumentAttribute: @(NSUTF8StringEncoding),
    };
    // Apply a bogus, easily detectable style rule to any feedback link, since
    // NSAttributedString doesn’t preserve the class attribute.
    NSMutableString *css = [NSMutableString stringWithString:
                            @"html { font-family: -apple-system, -apple-system-font, sans-serif; }"
                            @".mapbox-improve-map { -webkit-text-stroke-width: 1000px; }"];
    if (fontSize) {
        NSString *sizeRule = [NSString stringWithFormat:@"font-size: %.1fpx;", fontSize];
#if !TARGET_OS_IPHONE
        if (fontSize == [NSFont systemFontSizeForControlSize:NSMiniControlSize]) {
            sizeRule = @"font: -webkit-mini-control";
        } else if (fontSize == [NSFont systemFontSizeForControlSize:NSSmallControlSize]) {
            sizeRule = @"font: -webkit-small-control";
        } else if (fontSize == [NSFont systemFontSizeForControlSize:NSRegularControlSize]) {
            sizeRule = @"font: -webkit-control";
        }
#endif
        [css appendFormat:@"html { %@ }", sizeRule];
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

        // Remove the link, because it forces the text to be blue on macOS 10.12
        // and above.
        NSMutableAttributedString *unlinkedTitle = [title mutableCopy];
        [unlinkedTitle removeAttribute:NSLinkAttributeName range:unlinkedTitle.mgl_wholeRange];
        
        MGLAttributionInfo *info = [[MGLAttributionInfo alloc] initWithTitle:unlinkedTitle URL:value];
        info.feedbackLink = isFeedbackLink;
        [infos addObject:info];
    }];
    return infos;
}

+ (NSAttributedString *)attributedStringForAttributionInfos:(NS_ARRAY_OF(MGLAttributionInfo *) *)attributionInfos {
    NSMutableArray *titles = [NSMutableArray arrayWithCapacity:attributionInfos.count];
    for (MGLAttributionInfo *info in attributionInfos) {
        NSMutableAttributedString *title = info.title.mutableCopy;
        if (info.URL) {
            [title addAttribute:NSLinkAttributeName value:info.URL range:title.mgl_wholeRange];
        }
        [titles addObject:title];
    }
    return [titles mgl_attributedComponentsJoinedByString:@" "];
}

- (instancetype)initWithTitle:(NSAttributedString *)title URL:(NSURL *)URL {
    if (self = [super init]) {
        _title = title;
        _URL = URL;
    }
    return self;
}

- (id)copyWithZone:(nullable NSZone *)zone
{
    MGLAttributionInfo *info = [[[self class] allocWithZone:zone] initWithTitle:_title
                                                                            URL:_URL];
    info.feedbackLink = _feedbackLink;
    
    return info;
}

- (nullable NSURL *)feedbackURLAtCenterCoordinate:(CLLocationCoordinate2D)centerCoordinate zoomLevel:(double)zoomLevel {
    return [self feedbackURLForStyleURL:nil atCenterCoordinate:centerCoordinate zoomLevel:zoomLevel direction:0 pitch:0];
}

- (nullable NSURL *)feedbackURLForStyleURL:(nullable NSURL *)styleURL atCenterCoordinate:(CLLocationCoordinate2D)centerCoordinate zoomLevel:(double)zoomLevel direction:(CLLocationDirection)direction pitch:(CGFloat)pitch {
    if (!self.feedbackLink) {
        return nil;
    }
    
    NSURLComponents *components = [NSURLComponents componentsWithString:@"https://www.mapbox.com/feedback/"];
    components.fragment = [NSString stringWithFormat:@"/%.5f/%.5f/%.2f/%.1f/%i",
                           centerCoordinate.longitude, centerCoordinate.latitude, zoomLevel,
                           direction, (int)round(pitch)];
    
    NSURLQueryItem *referrerQueryItem = [NSURLQueryItem queryItemWithName:@"referrer"
                                                                    value:[NSBundle mgl_applicationBundleIdentifier]];
    NSMutableArray<NSURLQueryItem *> *queryItems = [NSMutableArray arrayWithObject:referrerQueryItem];
    if ([styleURL.scheme isEqualToString:@"mapbox"] && [styleURL.host isEqualToString:@"styles"]) {
        NSArray<NSString *> *stylePathComponents = styleURL.pathComponents;
        if (stylePathComponents.count >= 3) {
            [queryItems addObjectsFromArray:@[
                [NSURLQueryItem queryItemWithName:@"owner" value:stylePathComponents[1]],
                [NSURLQueryItem queryItemWithName:@"id" value:stylePathComponents[2]],
                [NSURLQueryItem queryItemWithName:@"access_token" value:[MGLAccountManager accessToken]],
                [NSURLQueryItem queryItemWithName:@"map_sdk_version" value:[NSBundle mgl_frameworkInfoDictionary][@"MGLSemanticVersionString"]],
            ]];
        }
    }
    components.queryItems = queryItems;
    
    return components.URL;
}

- (NSAttributedString *)titleWithStyle:(MGLAttributionInfoStyle)style
{
    NSString *openStreetMap = NSLocalizedStringWithDefaultValue(@"OSM_FULL_NAME", @"Foundation", nil, @"OpenStreetMap", @"OpenStreetMap full name attribution");
    NSString *OSM = NSLocalizedStringWithDefaultValue(@"OSM_SHORT_NAME", @"Foundation", nil, @"OSM", @"OpenStreetMap short name attribution");
    
    NSMutableAttributedString *title = [[NSMutableAttributedString alloc] initWithAttributedString:self.title];
    [title removeAttribute:NSUnderlineStyleAttributeName range:NSMakeRange(0, [title.string length])];
    
    BOOL isAbbreviated = (style == MGLAttributionInfoStyleShort);
    
    if ([title.string rangeOfString:@"OpenStreetMap"].location != NSNotFound) {
        [title.mutableString replaceOccurrencesOfString:@"OpenStreetMap" withString:isAbbreviated ? OSM : openStreetMap
                                                options:NSCaseInsensitiveSearch
                                                  range:NSMakeRange(0, [title.mutableString length])];
    }
    
    return title;
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
