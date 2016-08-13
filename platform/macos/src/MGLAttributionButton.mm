#import "MGLAttributionButton.h"
#import "MGLAttributionInfo.h"

#import "NSBundle+MGLAdditions.h"

@implementation MGLAttributionButton

- (instancetype)initWithAttributionInfo:(MGLAttributionInfo *)info {
    if (self = [super initWithFrame:NSZeroRect]) {
        self.bordered = NO;
        self.bezelStyle = NSRegularSquareBezelStyle;
        
        // Extract any prefix consisting of intellectual property symbols.
        NSScanner *scanner = [NSScanner scannerWithString:info.title];
        NSCharacterSet *symbolSet = [NSCharacterSet characterSetWithCharactersInString:@"©℗®℠™ &"];
        NSString *symbol;
        [scanner scanCharactersFromSet:symbolSet intoString:&symbol];
        NSString *title = [info.title substringFromIndex:symbol.length];
        
        // Start with the symbol prefix, sans underlining for aesthetic reasons. The whole string will be mini.
        NSMutableAttributedString *attributedTitle = [[NSMutableAttributedString alloc] initWithString:symbol ?: @"" attributes:@{
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
        
        _URL = info.URL;
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
