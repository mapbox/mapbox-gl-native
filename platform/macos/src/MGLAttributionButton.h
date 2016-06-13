#import <Cocoa/Cocoa.h>

/// Button that looks like a hyperlink and opens a URL.
@interface MGLAttributionButton : NSButton

/// Returns an `MGLAttributionButton` instance with the given title and URL.
- (instancetype)initWithTitle:(NSString *)title URL:(NSURL *)url;

/// The URL to open and display as a tooltip.
@property (nonatomic) NSURL *URL;

/// Opens the URL.
- (IBAction)openURL:(id)sender;

@end
