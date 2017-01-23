#import <Cocoa/Cocoa.h>

#import "MGLFoundation.h"
#import "MGLTypes.h"

NS_ASSUME_NONNULL_BEGIN

@class MGLAttributionInfo;

/// Button that looks like a hyperlink and opens a URL.
MGL_EXPORT
@interface MGLAttributionButton : NSButton

/// Returns an `MGLAttributionButton` instance with the given info.
- (instancetype)initWithAttributionInfo:(MGLAttributionInfo *)info;

/// The URL to open and display as a tooltip.
@property (nonatomic, readonly, nullable) NSURL *URL;

/// Opens the URL.
- (IBAction)openURL:(nullable id)sender;

@end

NS_ASSUME_NONNULL_END
