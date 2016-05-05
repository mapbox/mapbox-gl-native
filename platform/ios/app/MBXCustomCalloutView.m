#import "MBXCustomCalloutView.h"

static CGFloat const tipHeight = 10.0;
static CGFloat const tipWidth = 10.0;

@interface MBXCustomCalloutView ()

@property (strong, nonatomic) UILabel *mainLabel;

@end

@implementation MBXCustomCalloutView {
    NSString *_title;
}

@synthesize title = _title;

- (instancetype)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self)
    {
        self.backgroundColor = [UIColor clearColor];
        _mainLabel = [[UILabel alloc] initWithFrame: CGRectZero];
        _mainLabel.backgroundColor = [UIColor clearColor];
        
        [self addSubview: _mainLabel];
    }
    return self;
}


#pragma mark - API

- (void)presentCalloutFromRect:(CGRect)rect inView:(UIView *)view constrainedToView:(UIView *)constrainedView animated:(BOOL)animated
{
    [view addSubview:self];
    // prepare title label
    if (_title)
    {
        self.mainLabel.text = _title;
        [self.mainLabel sizeToFit];
    }
    // prepare our frame
    CGFloat frameWidth = self.mainLabel.bounds.size.width;
    CGFloat frameHeight = self.mainLabel.bounds.size.height * 2.0;
    CGFloat frameOriginX = rect.origin.x + (rect.size.width/2.0) - (frameWidth/2.0);
    CGFloat frameOriginY = rect.origin.y - frameHeight;
    self.frame = CGRectMake(frameOriginX, frameOriginY,
                            frameWidth, frameHeight);
}

- (void)dismissCalloutAnimated:(BOOL)animated
{
    if (self.superview)
    {
        [self removeFromSuperview];
    }
}

#pragma mark - internals

- (void)drawRect:(CGRect)rect
{
    UIColor *fillColor = [UIColor colorWithWhite:0.7 alpha:1.0];
    
    CGFloat tipLeft = rect.origin.x + (rect.size.width / 2.0) - (tipWidth / 2.0);
    CGPoint tipBottom = CGPointMake(rect.origin.x + (rect.size.width / 2.0), rect.origin.y +rect.size.height);
    CGFloat heightWithoutTip = rect.size.height - tipHeight;
    
    // draw the white background with tip
    CGContextRef ctxt = UIGraphicsGetCurrentContext();
    
    CGMutablePathRef tipPath = CGPathCreateMutable();
    CGPathMoveToPoint(tipPath, NULL, 0, 0);
    CGPathAddLineToPoint(tipPath, NULL, 0, heightWithoutTip);
    CGPathAddLineToPoint(tipPath, NULL, tipLeft, heightWithoutTip);
    CGPathAddLineToPoint(tipPath, NULL, tipBottom.x, tipBottom.y);
    CGPathAddLineToPoint(tipPath, NULL, tipLeft + tipWidth, heightWithoutTip);
    CGPathAddLineToPoint(tipPath, NULL, CGRectGetWidth(rect), heightWithoutTip);
    CGPathAddLineToPoint(tipPath, NULL, CGRectGetWidth(rect), 0);
    CGPathCloseSubpath(tipPath);
    
    [fillColor setFill];
    CGContextAddPath(ctxt, tipPath);
    CGContextFillPath(ctxt);
    CGPathRelease(tipPath);
}

@end
