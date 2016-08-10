#import "MGLCompactCalloutView.h"

// Set defaults for custom tip drawing
static CGFloat const tipHeight = 10.0;
static CGFloat const tipWidth = 20.0;

@interface MGLCompactCalloutView ()

@property (strong, nonatomic) UIButton *mainBody;

@end

@implementation MGLCompactCalloutView

+ (instancetype)calloutView
{
    return [[self alloc] init];
}

- (instancetype)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self)
    {
        self.backgroundColor = [UIColor clearColor];

        // Create and add a subview to hold the callout’s text
        UIButton *mainBody = [UIButton buttonWithType:UIButtonTypeSystem];
        mainBody.backgroundColor = [UIColor whiteColor];
        mainBody.contentEdgeInsets = UIEdgeInsetsMake(10.0, 10.0, 10.0, 10.0);
        mainBody.layer.cornerRadius = 4.0;
        self.mainBody = mainBody;

        [self addSubview:self.mainBody];
    }

    return self;
}

#pragma mark - MGLCalloutView API

- (void)presentCalloutFromRect:(CGRect)rect inView:(UIView *)view constrainedToView:(UIView *)constrainedView animated:(BOOL)animated
{
    // Do not show a callout if there is no title set
    if (!self.title)
    {
        return;
    }

    [view addSubview:self];

    // Prepare title label
    [self.mainBody setTitle:self.title forState:UIControlStateNormal];
    [self.mainBody sizeToFit];

    // Disable tapping and highlighting
    self.mainBody.userInteractionEnabled = NO;

    // Prepare our frame, adding extra space at the bottom for the tip
    CGFloat frameWidth = self.mainBody.bounds.size.width;
    CGFloat frameHeight = self.mainBody.bounds.size.height + tipHeight;
    CGFloat frameOriginX = rect.origin.x + (rect.size.width/2.0) - (frameWidth/2.0);
    CGFloat frameOriginY = rect.origin.y - frameHeight;
    self.frame = CGRectMake(frameOriginX, frameOriginY,
                            frameWidth, frameHeight);

    if (animated)
    {
        self.alpha = 0.0;

        [UIView animateWithDuration:0.2 animations:^{
            self.alpha = 1.0;
        }];
    }
}

- (void)dismissCalloutAnimated:(BOOL)animated
{
    if (self.superview)
    {
        if (animated)
        {
            [UIView animateWithDuration:0.2 animations:^{
                self.alpha = 0.0;
            } completion:^(BOOL finished) {
                [self removeFromSuperview];
            }];
        }
        else
        {
            [self removeFromSuperview];
        }
    }
}


#pragma mark - Custom view styling

- (void)drawRect:(CGRect)rect
{
    // Draw the pointed tip at the bottom
    CGFloat tipLeft = rect.origin.x + (rect.size.width / 2.0) - (tipWidth / 2.0);
    CGPoint tipBottom = CGPointMake(rect.origin.x + (rect.size.width / 2.0), rect.origin.y + rect.size.height);
    CGFloat heightWithoutTip = rect.size.height - tipHeight - 1;

    CGContextRef currentContext = UIGraphicsGetCurrentContext();

    CGMutablePathRef tipPath = CGPathCreateMutable();
    CGPathMoveToPoint(tipPath, NULL, tipLeft, heightWithoutTip);
    CGPathAddLineToPoint(tipPath, NULL, tipBottom.x, tipBottom.y);
    CGPathAddLineToPoint(tipPath, NULL, tipLeft + tipWidth, heightWithoutTip);
    CGPathCloseSubpath(tipPath);

    [self.mainBody.backgroundColor setFill];
    CGContextAddPath(currentContext, tipPath);
    CGContextFillPath(currentContext);
    CGPathRelease(tipPath);
}

@end
