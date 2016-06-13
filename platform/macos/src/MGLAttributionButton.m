#import "MGLAttributionButton.h"

#import "NSBundle+MGLAdditions.h"

@implementation MGLAttributionButton {
    NSTrackingRectTag _trackingAreaTag;
}

- (instancetype)initWithTitle:(NSString *)title URL:(NSURL *)url {
    if (self = [super initWithFrame:NSZeroRect]) {
        self.bordered = NO;
        self.bezelStyle = NSRegularSquareBezelStyle;
        
        // Start with a copyright symbol. The whole string will be mini.
        NSMutableAttributedString *attributedTitle = [[NSMutableAttributedString alloc] initWithString:NSLocalizedStringWithDefaultValue(@"COPYRIGHT_PREFIX", nil, nil, @"© ", @"Copyright notice prefix") attributes:@{
            NSFontAttributeName: [NSFont systemFontOfSize:[NSFont systemFontSizeForControlSize:NSMiniControlSize]],
        }];
        // Append the specified title, underlining it like a hyperlink.
        [attributedTitle appendAttributedString:
         [[NSAttributedString alloc] initWithString:title
                                         attributes:@{
            NSFontAttributeName: [NSFont systemFontOfSize:[NSFont systemFontSizeForControlSize:NSMiniControlSize]],
            NSUnderlineStyleAttributeName: @(NSUnderlineStyleSingle),
        }]];
        self.attributedTitle = attributedTitle;
        [self sizeToFit];
        
        _URL = url;
        self.toolTip = _URL.absoluteString;
        
        self.target = self;
        self.action = @selector(openURL:);
    }
    return self;
}

- (BOOL)wantsLayer {
    return YES;
}

- (void)resetCursorRects {
    // The whole button gets a pointing hand cursor, just like a hyperlink.
    [self addCursorRect:self.bounds cursor:[NSCursor pointingHandCursor]];
}

- (IBAction)openURL:(__unused id)sender {
    [[NSWorkspace sharedWorkspace] openURL:self.URL];
}

@end
