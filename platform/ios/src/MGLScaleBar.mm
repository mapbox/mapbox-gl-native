#import <Mapbox/Mapbox.h>
#import "MGLScaleBar.h"

static const CGFloat MGLFeetPerMile = 5280;

struct MGLRow {
    CLLocationDistance distance;
    NSUInteger numberOfBars;
};

static const MGLRow MGLMetricTable[] = {
    {.distance = 1, .numberOfBars = 2},
    {.distance = 2, .numberOfBars = 2},
    {.distance = 4, .numberOfBars = 2},
    {.distance = 10, .numberOfBars = 2},
    {.distance = 20, .numberOfBars = 2},
    {.distance = 50, .numberOfBars = 2},
    {.distance = 75, .numberOfBars = 3},
    {.distance = 100, .numberOfBars = 2},
    {.distance = 150, .numberOfBars = 2},
    {.distance = 200, .numberOfBars = 2},
    {.distance = 300, .numberOfBars = 3},
    {.distance = 500, .numberOfBars = 2},
    {.distance = 1000, .numberOfBars = 2},
    {.distance = 1500, .numberOfBars = 2},
    {.distance = 3000, .numberOfBars = 3},
    {.distance = 5000, .numberOfBars = 2},
    {.distance = 10000, .numberOfBars = 2},
    {.distance = 20000, .numberOfBars = 2},
    {.distance = 30000, .numberOfBars = 3},
    {.distance = 50000, .numberOfBars = 2},
    {.distance = 100000, .numberOfBars = 2},
    {.distance = 200000, .numberOfBars = 2},
    {.distance = 300000, .numberOfBars = 3},
    {.distance = 400000, .numberOfBars = 2},
    {.distance = 500000, .numberOfBars = 2},
    {.distance = 600000, .numberOfBars = 3},
    {.distance = 800000, .numberOfBars = 2},
    {.distance = 1000000, .numberOfBars = 2},
    {.distance = 1200000, .numberOfBars = 2},
    {.distance = 1600000, .numberOfBars = 2},
    {.distance = 2000000, .numberOfBars = 2},
    {.distance = 3500000, .numberOfBars = 2},
    {.distance = 5000000, .numberOfBars = 2},
};

static const MGLRow MGLImperialTable[] ={
    {.distance = 4, .numberOfBars = 2},
    {.distance = 6, .numberOfBars = 2},
    {.distance = 10, .numberOfBars = 2},
    {.distance = 20, .numberOfBars = 2},
    {.distance = 30, .numberOfBars = 2},
    {.distance = 50, .numberOfBars = 2},
    {.distance = 75, .numberOfBars = 3},
    {.distance = 100, .numberOfBars = 2},
    {.distance = 200, .numberOfBars = 2},
    {.distance = 300, .numberOfBars = 3},
    {.distance = 400, .numberOfBars = 2},
    {.distance = 600, .numberOfBars = 3},
    {.distance = 800, .numberOfBars = 2},
    {.distance = 1000, .numberOfBars = 2},
    {.distance = 0.25f*MGLFeetPerMile, .numberOfBars = 2},
    {.distance = 0.5f*MGLFeetPerMile, .numberOfBars = 2},
    {.distance = 1*MGLFeetPerMile, .numberOfBars = 2},
    {.distance = 2*MGLFeetPerMile, .numberOfBars = 2},
    {.distance = 3*MGLFeetPerMile, .numberOfBars = 3},
    {.distance = 4*MGLFeetPerMile, .numberOfBars = 2},
    {.distance = 8*MGLFeetPerMile, .numberOfBars = 2},
    {.distance = 12*MGLFeetPerMile, .numberOfBars = 2},
    {.distance = 15*MGLFeetPerMile, .numberOfBars = 3},
    {.distance = 20*MGLFeetPerMile, .numberOfBars = 2},
    {.distance = 30*MGLFeetPerMile, .numberOfBars = 3},
    {.distance = 40*MGLFeetPerMile, .numberOfBars = 2},
    {.distance = 80*MGLFeetPerMile, .numberOfBars = 2},
    {.distance = 120*MGLFeetPerMile, .numberOfBars = 2},
    {.distance = 200*MGLFeetPerMile, .numberOfBars = 2},
    {.distance = 300*MGLFeetPerMile, .numberOfBars = 3},
    {.distance = 400*MGLFeetPerMile, .numberOfBars = 2},
    {.distance = 800*MGLFeetPerMile, .numberOfBars = 2},
    {.distance = 1000*MGLFeetPerMile, .numberOfBars = 2},
    {.distance = 2000*MGLFeetPerMile, .numberOfBars = 2},
    {.distance = 3000*MGLFeetPerMile, .numberOfBars = 3},
    {.distance = 4000*MGLFeetPerMile, .numberOfBars = 2},
};

@class MGLScaleBarLabel;

@interface MGLScaleBar()
@property (nonatomic, assign, getter=isVisible) BOOL visible;
@property (nonatomic) NSArray<MGLScaleBarLabel *> *labels;
@property (nonatomic) NSArray<UIView *> *bars;
@property (nonatomic) UIView *containerView;
@property (nonatomic) MGLDistanceFormatter *formatter;
@property (nonatomic, assign) MGLRow row;
@property (nonatomic) UIColor *primaryColor;
@property (nonatomic) UIColor *secondaryColor;
@property (nonatomic) CALayer *borderLayer;
@property (nonatomic, assign) CGFloat borderWidth;
@end

static const CGFloat MGLBarHeight = 4;
static const CGFloat MGLFeetPerMeter = 3.28084;

@interface MGLScaleBarLabel : UILabel
@end

@implementation MGLScaleBarLabel

- (void)drawTextInRect:(CGRect)rect {
    CGSize shadowOffset = self.shadowOffset;
    UIColor *textColor = self.textColor;
    
    CGContextRef context = UIGraphicsGetCurrentContext();
    CGContextSetLineWidth(context, 2);
    CGContextSetLineJoin(context, kCGLineJoinRound);
    
    CGContextSetTextDrawingMode(context, kCGTextStroke);
    self.textColor = [UIColor whiteColor];
    [super drawTextInRect:rect];
    
    CGContextSetTextDrawingMode(context, kCGTextFill);
    self.textColor = textColor;
    self.shadowOffset = CGSizeMake(0, 0);
    [super drawTextInRect:rect];
    
    self.shadowOffset = shadowOffset;
}

@end

@implementation MGLScaleBar

- (instancetype)initWithCoder:(NSCoder *)decoder {
    if (self = [super initWithCoder:decoder]) {
        [self commonInit];
    }
    return self;
}

- (instancetype)initWithFrame:(CGRect)frame {
    if (self = [super initWithFrame:frame]) {
        [self commonInit];
    }
    return self;
}

- (void)commonInit {
    _visible = NO;
    _primaryColor = [UIColor colorWithRed:18.0/255.0 green:45.0/255.0 blue:17.0/255.0 alpha:1];
    _secondaryColor = [UIColor colorWithRed:247.0/255.0 green:247.0/255.0 blue:247.0/255.0 alpha:1];
    _borderWidth = 1.0f;
    
    self.clipsToBounds = NO;
    self.alpha = 0;
    
    _containerView = [[UIView alloc] init];
    _containerView.clipsToBounds = YES;
    _containerView.backgroundColor = self.secondaryColor;
    [self addSubview:_containerView];
    
    _borderLayer = [CAShapeLayer layer];
    _borderLayer.borderColor = [self.primaryColor CGColor];
    _borderLayer.borderWidth = 1.0f / [[UIScreen mainScreen] scale];
    
    [_containerView.layer addSublayer:_borderLayer];
    
    _formatter = [[MGLDistanceFormatter alloc] init];
}

#pragma mark - Dimensions

- (CGSize)intrinsicContentSize {
    return CGSizeMake(self.actualWidth, 16);
}

- (CGFloat)actualWidth {
    return self.row.distance / [self unitsPerPoint];
}

- (CGFloat)minimumWidth {
    CGFloat fullWidth = CGRectGetWidth(self.superview.bounds);
    CGFloat padding = CGRectGetMinX(self.frame);
    CGFloat width = fullWidth - padding;
    return floorf((width / 2.0) / 3.0);
}

- (CGFloat)maximumWidth {
    CGFloat fullWidth = CGRectGetWidth(self.superview.bounds);
    CGFloat padding = CGRectGetMinX(self.frame);
    return floorf(fullWidth / 2 - padding);
}

- (CGFloat)unitsPerPoint {
    return [self usesMetricSystem] ? self.metersPerPoint : self.metersPerPoint * MGLFeetPerMeter;
}

#pragma mark - Convenient methods

- (MGLRow)preferredRow {
    CLLocationDistance maximumDistance = [self maximumWidth] * [self unitsPerPoint];
    MGLRow row;
    
    BOOL useMetric = [self usesMetricSystem];
    NSUInteger count = useMetric
    ? sizeof(MGLMetricTable) / sizeof(MGLMetricTable[0])
    : sizeof(MGLImperialTable) / sizeof(MGLImperialTable[0]);
    
    for (NSUInteger i = 0; i < count; i++) {
        CLLocationDistance distance = useMetric ? MGLMetricTable[i].distance : MGLImperialTable[i].distance;
        if (distance <= maximumDistance) {
            row = useMetric ? MGLMetricTable[i] : MGLImperialTable[i];
        } else {
            break;
        }
    }
    
    return row;
}

- (BOOL)usesMetricSystem {
    NSLocale *locale = [NSLocale currentLocale];
    return [[locale objectForKey:NSLocaleUsesMetricSystem] boolValue];
}

- (void)fadeIn {
    if (self.isVisible) {
        return;
    }

    self.visible = YES;
    
    [UIView animateWithDuration:.2 delay:0 options:UIViewAnimationOptionBeginFromCurrentState animations:^{
        self.alpha = 1.0;
    } completion:nil];
}

- (void)fadeOut {
    if (!self.isVisible) {
        return;
    }
    
    self.visible = NO;
    
    [UIView animateWithDuration:.3 delay:0.7 options:UIViewAnimationOptionBeginFromCurrentState animations:^{
        self.alpha = 0.0;
    } completion:nil];
}

#pragma mark - Setters

- (void)setMetersPerPoint:(CLLocationDistance)metersPerPoint {
    if (_metersPerPoint == metersPerPoint) {
        return;
    }
    
    [self fadeIn];
    
    _metersPerPoint = metersPerPoint;
    
    self.row = [self preferredRow];
    
    [self invalidateIntrinsicContentSize];
    [self setNeedsLayout];
}

- (void)setRow:(MGLRow)row {
    if (_row.distance != row.distance) {
        [_bars makeObjectsPerformSelector:@selector(removeFromSuperview)];
        [_labels makeObjectsPerformSelector:@selector(removeFromSuperview)];
        _bars = nil;
        _labels = nil;
        _row = row;
    }
}

#pragma mark - Views

- (NSArray<UIView *> *)bars {
    if (!_bars) {
        NSMutableArray *bars = [NSMutableArray array];
        for (NSUInteger i = 0; i < self.row.numberOfBars; i++) {
            UIView *bar = [[UIView alloc] init];
            [bars addObject:bar];
            [self.containerView addSubview:bar];
        }
        _bars = bars;
    }
    return _bars;
}

- (NSArray<UILabel *> *)labels {
    if (!_labels) {
        NSCharacterSet *characterSet = [[NSCharacterSet decimalDigitCharacterSet] invertedSet];
        NSMutableArray *labels = [NSMutableArray array];
        
        for (NSUInteger i = 0; i <= self.row.numberOfBars; i++) {
            UILabel *label = [[MGLScaleBarLabel alloc] init];
            label.font = [UIFont systemFontOfSize:8 weight:UIFontWeightMedium];
            label.text = [[[self.formatter stringFromDistance:0] componentsSeparatedByCharactersInSet:characterSet] componentsJoinedByString:@""];
            label.clipsToBounds = NO;
            [label setNeedsDisplay];
            [label sizeToFit];
            [labels addObject:label];
            [self addSubview:label];
        }
        _labels = labels;
    }
    return _labels;
}

#pragma mark - Layout

- (void)layoutSubviews {
    [super layoutSubviews];
    
    if (!self.row.numberOfBars) {
        // Current distance is not within allowed range
        return;
    }
    
    [self updateLabels];
    [self layoutBars];
    [self layoutLabels];
}

- (void)layoutBars {
    CGFloat barWidth = (CGRectGetWidth(self.bounds) - self.borderWidth * 2.0f) / self.bars.count;
    
    NSUInteger i = 0;
    for (UIView *bar in self.bars) {
        CGFloat xPosition = barWidth * i + self.borderWidth;
        bar.backgroundColor = (i % 2 == 0) ? self.primaryColor : self.secondaryColor;
        bar.frame = CGRectMake(xPosition, self.borderWidth, barWidth, MGLBarHeight);
        i++;
    }
    
    self.containerView.frame = CGRectMake(CGRectGetMinX(self.bars.firstObject.frame),
                                          CGRectGetMaxY(self.bounds)-MGLBarHeight,
                                          self.actualWidth,
                                          MGLBarHeight+self.borderWidth*2);
    
    [CATransaction begin];
    [CATransaction setDisableActions:YES];
    self.borderLayer.frame = CGRectInset(self.containerView.bounds, self.borderWidth, self.borderWidth);
    self.borderLayer.zPosition = FLT_MAX;
    [CATransaction commit];
}

- (void)layoutLabels {
    CGFloat barWidth = self.bounds.size.width / self.bars.count;
    NSUInteger i = 0;
    for (MGLScaleBarLabel *label in self.labels) {
        CGFloat xPosition = barWidth * i - CGRectGetMidX(label.bounds) + self.borderWidth;
        label.frame = CGRectMake(xPosition, 0,
                                 CGRectGetWidth(label.bounds),
                                 CGRectGetHeight(label.bounds));
        i++;
    }
}

- (void)updateLabels {
    NSArray *labels = [self.labels subarrayWithRange:NSMakeRange(1, self.labels.count-1)];
    BOOL useMetric = [self usesMetricSystem];
    NSUInteger i = 0;
    
    for (MGLScaleBarLabel *label in labels) {
        CLLocationDistance barDistance = (self.row.distance / self.row.numberOfBars) * (i + 1);
        
        if (!useMetric) {
            barDistance /= MGLFeetPerMeter;
        }
        
        label.text = [self.formatter stringFromDistance:barDistance];
        [label setNeedsDisplay];
        [label sizeToFit];
        i++;
    }
}

@end
