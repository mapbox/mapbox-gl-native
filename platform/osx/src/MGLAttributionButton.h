#import <Cocoa/Cocoa.h>

@interface MGLAttributionButton : NSButton

- (instancetype)initWithTitle:(NSString *)title URL:(NSURL *)url;

@property (nonatomic) NSURL *URL;

- (IBAction)openURL:(id)sender;

@end
