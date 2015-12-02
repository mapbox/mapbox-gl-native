#import "MGLAttributionButton.h"

@implementation MGLAttributionButton {
    NSTrackingRectTag _trackingAreaTag;
}

- (instancetype)initWithTitle:(NSString *)title URL:(NSURL *)url {
    if (self = [super initWithFrame:NSZeroRect]) {
        self.bordered = NO;
        self.bezelStyle = NSRegularSquareBezelStyle;
        
        NSMutableAttributedString *attributedTitle = [[NSMutableAttributedString alloc] initWithString:@"© "
                                                                                            attributes:@{
            NSFontAttributeName: [NSFont systemFontOfSize:[NSFont systemFontSizeForControlSize:NSMiniControlSize]],
        }];
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
    [self addCursorRect:self.bounds cursor:[NSCursor pointingHandCursor]];
}

- (IBAction)openURL:(__unused id)sender {
    [[NSWorkspace sharedWorkspace] openURL:self.URL];
}

@end
