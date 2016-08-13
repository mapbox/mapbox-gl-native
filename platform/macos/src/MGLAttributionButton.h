#import <Cocoa/Cocoa.h>

@class MGLAttributionInfo;

/// Button that looks like a hyperlink and opens a URL.
@interface MGLAttributionButton : NSButton

/// Returns an `MGLAttributionButton` instance with the given info.
- (instancetype)initWithAttributionInfo:(MGLAttributionInfo *)info;

/// The URL to open and display as a tooltip.
@property (nonatomic) NSURL *URL;

/// Opens the URL.
- (IBAction)openURL:(id)sender;

@end
