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
};

@class MGLScaleBarLabel;

@interface MGLScaleBar()
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
    _primaryColor = [UIColor colorWithRed:18.0/255.0 green:45.0/255.0 blue:17.0/255.0 alpha:1];
    _secondaryColor = [UIColor colorWithRed:247.0/255.0 green:247.0/255.0 blue:247.0/255.0 alpha:1];
    _borderWidth = 1.0f;
    
    self.clipsToBounds = NO;
    self.hidden = YES;
    
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
    CGFloat width = self.row.distance / [self unitsPerPoint];
    return !isnan(width) ? width : 0;
}

- (CGFloat)maximumWidth {
    CGFloat fullWidth = CGRectGetWidth(self.superview.bounds);
    CGFloat padding = [self usesRightToLeftLayout] ? fullWidth - CGRectGetMaxX(self.frame) : CGRectGetMinX(self.frame);
    return floorf(fullWidth / 2 - padding);
}

- (CGFloat)unitsPerPoint {
    return [self usesMetricSystem] ? self.metersPerPoint : self.metersPerPoint * MGLFeetPerMeter;
}

#pragma mark - Convenience methods

- (BOOL)usesRightToLeftLayout {
    // semanticContentAttribute is iOS 9+
    if ([self.superview respondsToSelector:@selector(semanticContentAttribute)]) {
        return [UIView userInterfaceLayoutDirectionForSemanticContentAttribute:self.superview.semanticContentAttribute] == UIUserInterfaceLayoutDirectionRightToLeft;
    } else {
        return UIApplication.sharedApplication.userInterfaceLayoutDirection == UIUserInterfaceLayoutDirectionRightToLeft;
    }
}

- (BOOL)usesMetricSystem {
    NSLocale *locale = [NSLocale currentLocale];
    return [[locale objectForKey:NSLocaleUsesMetricSystem] boolValue];
}

- (MGLRow)preferredRow {
    CLLocationDistance maximumDistance = [self maximumWidth] * [self unitsPerPoint];
    
    BOOL useMetric = [self usesMetricSystem];
    MGLRow row = useMetric ? MGLMetricTable[0] : MGLImperialTable[0];
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

#pragma mark - Setters

- (void)setMetersPerPoint:(CLLocationDistance)metersPerPoint {
    if (_metersPerPoint == metersPerPoint) {
        return;
    }
    
    _metersPerPoint = metersPerPoint;
    
    [self updateVisibility];
    
    self.row = [self preferredRow];
    
    CGSize size = self.intrinsicContentSize;
    self.frame = CGRectMake(CGRectGetMinX(self.frame),
                            CGRectGetMinY(self.frame),
                            size.width,
                            size.height);
}

- (void)updateVisibility {
    BOOL metric = [self usesMetricSystem];
    
    NSUInteger count = metric
    ? sizeof(MGLMetricTable) / sizeof(MGLMetricTable[0])
    : sizeof(MGLImperialTable) / sizeof(MGLImperialTable[0]);
    
    CLLocationDistance maximumDistance = [self maximumWidth] * [self unitsPerPoint];
    CLLocationDistance allowedDistance = metric
    ? MGLMetricTable[count-1].distance
    : MGLImperialTable[count-1].distance;
    
    CGFloat alpha = maximumDistance > allowedDistance ? .0f : 1.0f;
    
    if (self.alpha != alpha) {
        [UIView animateWithDuration:.2f delay:0 options:UIViewAnimationOptionBeginFromCurrentState animations:^{
            self.alpha = alpha;
        } completion:nil];
    }
}

- (void)setRow:(MGLRow)row {
    if (_row.distance == row.distance) {
        return;
    }
    
    _row = row;
    [_bars makeObjectsPerformSelector:@selector(removeFromSuperview)];
    [_labels makeObjectsPerformSelector:@selector(removeFromSuperview)];
    _bars = nil;
    _labels = nil;
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
        NSDecimalNumber *zeroNumber = [NSDecimalNumber decimalNumberWithString:@"0"];
        NSNumberFormatter *formatter = [[NSNumberFormatter alloc] init];
        NSMutableArray *labels = [NSMutableArray array];
        
        for (NSUInteger i = 0; i <= self.row.numberOfBars; i++) {
            UILabel *label = [[MGLScaleBarLabel alloc] init];
            label.font = [UIFont systemFontOfSize:8 weight:UIFontWeightMedium];
            label.text = [formatter stringFromNumber:zeroNumber];
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

- (void)layoutBars {
    CGFloat barWidth = round((CGRectGetWidth(self.bounds) - self.borderWidth * 2.0f) / self.bars.count);
    
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
    CGFloat barWidth = round(self.bounds.size.width / self.bars.count);
    BOOL RTL = [self usesRightToLeftLayout];
    NSUInteger i = RTL ? self.bars.count : 0;
    for (MGLScaleBarLabel *label in self.labels) {
        CGFloat xPosition = round(barWidth * i - CGRectGetMidX(label.bounds) + self.borderWidth);
        label.frame = CGRectMake(xPosition, 0,
                                 CGRectGetWidth(label.bounds),
                                 CGRectGetHeight(label.bounds));
        i = RTL ? i-1 : i+1;
    }
}

@end
