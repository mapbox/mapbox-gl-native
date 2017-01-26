#import "MBXCustomCalloutView.h"

static CGFloat const tipHeight = 10.0;
static CGFloat const tipWidth = 10.0;

@interface MBXCustomCalloutView ()

@property (strong, nonatomic) UILabel *mainLabel;

@end

@implementation MBXCustomCalloutView {
    id <MGLAnnotation> _representedObject;
    UIView *_leftAccessoryView;
    UIView *_rightAccessoryView;
    __weak id <MGLCalloutViewDelegate> _delegate;
}

@synthesize representedObject = _representedObject;
@synthesize anchoredToAnnotation = _anchoredToAnnotation;
@synthesize dismissesAutomatically = _dismissesAutomatically;
@synthesize leftAccessoryView = _leftAccessoryView;
@synthesize rightAccessoryView = _rightAccessoryView;
@synthesize delegate = _delegate;

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
    if ([self.delegate respondsToSelector:@selector(calloutViewWillAppear:)])
    {
        [self.delegate performSelector:@selector(calloutViewWillAppear:) withObject:self];
    }

    [view addSubview:self];
    // prepare title label
    if ([self.representedObject respondsToSelector:@selector(title)])
    {
        self.mainLabel.text = self.representedObject.title;
        [self.mainLabel sizeToFit];
    }
    // prepare our frame
    CGFloat frameWidth = self.mainLabel.bounds.size.width;
    CGFloat frameHeight = self.mainLabel.bounds.size.height * 2.0;
    CGFloat frameOriginX = rect.origin.x + (rect.size.width/2.0) - (frameWidth/2.0);
    CGFloat frameOriginY = rect.origin.y - frameHeight;
    self.frame = CGRectMake(frameOriginX, frameOriginY,
                            frameWidth, frameHeight);

    if ([self.delegate respondsToSelector:@selector(calloutViewDidAppear:)])
    {
        [self.delegate performSelector:@selector(calloutViewDidAppear:) withObject:self];
    }
}

- (void)setCenter:(CGPoint)center {
    center.y = center.y - CGRectGetMidY(self.bounds);
    [super setCenter:center];
}

- (void)dismissCalloutAnimated:(BOOL)animated
{
    if (self.superview)
        [self removeFromSuperview];
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
