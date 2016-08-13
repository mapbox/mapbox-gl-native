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

+ (NS_ARRAY_OF(MGLAttributionInfo *) *)attributionInfosFromHTMLString:(NSString *)htmlString {
    NSMutableArray *infos = [NSMutableArray array];
    NSData *htmlData = [htmlString dataUsingEncoding:NSUTF8StringEncoding];
#if TARGET_OS_IPHONE
    NSAttributedString *attributedString = [[NSAttributedString alloc] initWithData:htmlData
                                                                            options:@{
        NSDocumentTypeDocumentAttribute: NSHTMLTextDocumentType,
        NSCharacterEncodingDocumentAttribute: @(NSUTF8StringEncoding),
    }
                                                                 documentAttributes:nil
                                                                              error:NULL];
#else
    NSAttributedString *attributedString = [[NSAttributedString alloc] initWithHTML:htmlData documentAttributes:nil];
#endif
    NSString *string = attributedString.string;
    [attributedString enumerateAttribute:NSLinkAttributeName
                                 inRange:attributedString.mgl_wholeRange
                                 options:0
                              usingBlock:^(id _Nullable value, NSRange range, BOOL * _Nonnull stop) {
        if (!value) {
            return;
        }
        
        // Omit the Map Feedback link because the SDK already provides the appropriate UI for giving feedback.
        NSCAssert([value isKindOfClass:[NSURL class]], @"URL attribute must be an NSURL.");
        if ([value isEqual:[NSURL URLWithString:MGLAttributionFeedbackURLString]]) {
            return;
        }
        
        NSString *title = [[string substringWithRange:range] stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceCharacterSet]];
        MGLAttributionInfo *info = [[MGLAttributionInfo alloc] initWithTitle:title URL:value];
        [infos addObject:info];
    }];
    return infos;
}

- (instancetype)initWithTitle:(NSString *)title URL:(NSURL *)URL {
    if (self = [super init]) {
        _title = title;
        _URL = URL;
    }
    return self;
}

@end
