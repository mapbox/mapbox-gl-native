#import "MBXFrameTimeGraphView.h"

const CGFloat MBXFrameTimeExaggeration = 4.f * 1000.f;
const CGFloat MBXFrameTimeBarWidth = 4.f;

@interface MBXFrameTimeGraphView ()

@property (nonatomic) CAScrollLayer *scrollLayer;
@property (nonatomic) CAShapeLayer *thresholdLayer;
@property (nonatomic) CGFloat currentX;
@property (nonatomic) NSMutableArray<CAShapeLayer *> *barLayers;

@property (nonatomic) UIColor *safeColor;
@property (nonatomic) UIColor *warningColor;
@property (nonatomic) UIColor *dangerColor;

@end

@implementation MBXFrameTimeGraphView

- (instancetype)init {
    if (self = [super init]) {
        [self commonInit];
    }
    return self;
}

- (instancetype)initWithCoder:(NSCoder *)aDecoder {
    if (self = [super initWithCoder:aDecoder]) {
        [self commonInit];
    }
    return self;
}

- (void)commonInit {
    self.userInteractionEnabled = NO;
    self.layer.opacity = 0.9f;

    self.scrollLayer = [CAScrollLayer layer];
    self.scrollLayer.scrollMode = kCAScrollHorizontally;
    self.scrollLayer.masksToBounds = YES;
    [self.layer addSublayer:self.scrollLayer];

    self.thresholdLayer = [CAShapeLayer layer];
    self.thresholdLayer.fillColor = [UIColor darkGrayColor].CGColor;
    [self.layer addSublayer:self.thresholdLayer];

    self.barLayers = [NSMutableArray array];

    self.safeColor = [UIColor colorWithRed:(CGFloat)(0.f/255.f) green:(CGFloat)(190.f/255.f) blue:(CGFloat)(123.f/255.f) alpha:1.f];
    self.warningColor = [UIColor colorWithRed:(CGFloat)(255.f/255.f) green:(CGFloat)(154.f/255.f) blue:(CGFloat)(82.f/255.f) alpha:1.f];
    self.dangerColor = [UIColor colorWithRed:(CGFloat)(255.f/255.f) green:(CGFloat)(91.f/255.f) blue:(CGFloat)(86.f/255.f) alpha:1.f];
}

- (void)layoutSubviews {
    [super layoutSubviews];

    if (!CGRectEqualToRect(self.scrollLayer.frame, self.bounds)) {
        self.scrollLayer.frame = self.bounds;

        CGRect thresholdLineRect = CGRectMake(0, self.frame.size.height - [self renderDurationTargetMilliseconds], self.frame.size.width, 1);
        
        {
            CGPathRef path = CGPathCreateWithRect(thresholdLineRect, nil);        
            self.thresholdLayer.path = path;
            CGPathRelease(path);
        }
    }
}

- (void)updatePathWithFrameDuration:(CFTimeInterval)frameDuration {
    [CATransaction begin];
    [CATransaction setDisableActions:YES];

    self.currentX += MBXFrameTimeBarWidth;

    CAShapeLayer *bar = [self barWithFrameDuration:frameDuration];
    bar.position = CGPointMake(self.currentX, self.frame.size.height);

    [self.scrollLayer addSublayer:bar];
    [self.barLayers addObject:bar];

    [self.scrollLayer scrollToPoint:CGPointMake(self.currentX - self.frame.size.width, 0)];

    [self removeStaleBars];

    [CATransaction commit];
}

- (CGFloat)renderDurationTargetMilliseconds {
    CGFloat maximumFramesPerSecond;
    if (@available(iOS 10.3, *)) {
        maximumFramesPerSecond = UIScreen.mainScreen.maximumFramesPerSecond;
    } else {
        // Not always strictly accurate, but works as an expedient approximation.
        maximumFramesPerSecond = 60;
    }

    CGFloat target = (1.0 / maximumFramesPerSecond) * MBXFrameTimeExaggeration;
    return [self roundedFloat:target];
}

- (CGFloat)roundedFloat:(CGFloat)f {
#if TARGET_OS_IPHONE || TARGET_OS_SIMULATOR
    CGFloat scaleFactor = [UIScreen mainScreen].nativeScale;
#elif TARGET_OS_MAC
    CGFloat scaleFactor = [NSScreen mainScreen].backingScaleFactor;
#endif
    return round(f * scaleFactor) / scaleFactor;
}

- (CAShapeLayer *)barWithFrameDuration:(CFTimeInterval)frameDuration {
    CAShapeLayer *bar = [CAShapeLayer layer];

    CGRect barRect = CGRectMake(0, 0, MBXFrameTimeBarWidth, -(fminf(frameDuration * MBXFrameTimeExaggeration, self.frame.size.height)));
    UIBezierPath *barPath = [UIBezierPath bezierPathWithRect:barRect];
    bar.path = barPath.CGPath;
    bar.fillColor = [self colorForFrameDuration:frameDuration].CGColor;

    return bar;
}

- (UIColor *)colorForFrameDuration:(CFTimeInterval)frameDuration {
    CGFloat renderDurationTargetMilliseconds = [self renderDurationTargetMilliseconds];
    frameDuration *= MBXFrameTimeExaggeration;

    if (frameDuration < renderDurationTargetMilliseconds && frameDuration > (renderDurationTargetMilliseconds * 0.75)) {
        return self.warningColor;
    } else if (frameDuration > renderDurationTargetMilliseconds) {
        return self.dangerColor;
    } else {
        return self.safeColor;
    }
}

- (void)removeStaleBars {
    if (self.barLayers.count > (self.frame.size.width / MBXFrameTimeBarWidth * 3)) {
        NSRange staleBarsRange = NSMakeRange(0, self.frame.size.width / MBXFrameTimeBarWidth);
        NSArray *staleBars = [self.barLayers subarrayWithRange:staleBarsRange];
        [staleBars makeObjectsPerformSelector:@selector(removeFromSuperlayer)];
        [self.barLayers removeObjectsInRange:staleBarsRange];
    }
}

@end
