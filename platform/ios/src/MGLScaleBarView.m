#import <Mapbox/Mapbox.h>
#import "MGLScaleBarView.h"


@class MGLScaleBarLabel;

@interface MGLScaleBarView()
@property (nonatomic, assign, getter=isVisible) BOOL visible;
@property (nonatomic) NSArray<MGLScaleBarLabel *> *labels;
@property (nonatomic) NSArray<UIView *> *bars;
@property (nonatomic) UIView *containerView;
@property (nonatomic) MGLDistanceFormatter *formatter;
@property (nonatomic) NSArray<NSArray<NSNumber *> *> *metricTable;
@property (nonatomic) NSArray<NSArray<NSNumber *> *> *imperialTable;
@property (nonatomic) NSArray<NSNumber*> *row;
@property (nonatomic) UIColor *primaryColor;
@property (nonatomic) UIColor *secondaryColor;
@property (nonatomic) CALayer *borderLayer;
@property (nonatomic, assign) CGFloat borderWidth;
@end

static const CGFloat MGLBarHeight = 4;
static const CGFloat MGLFeetPerMeter = 3.28084;
static const CGFloat MGLFeetPerMile = 5280;

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

@implementation MGLScaleBarView

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
    _row = @[@0, @0];
    
    // @[meters, numberOfBars]
    _metricTable = @[@[@1, @2],
                     @[@2, @2],
                     @[@4, @2],
                     @[@10, @2],
                     @[@20, @2],
                     @[@50, @2],
                     @[@75, @3],
                     @[@100, @2],
                     @[@150, @3],
                     @[@200, @2],
                     @[@300, @3],
                     @[@500, @2],
                     @[@1000, @2],
                     @[@1500, @3],
                     @[@3000, @3],
                     @[@5000, @2],
                     @[@10000, @2],
                     @[@20000, @2],
                     @[@30000, @3],
                     @[@50000, @2],
                     @[@100000, @2],
                     @[@200000, @2],
                     @[@300000, @3],
                     @[@400000, @2],
                     @[@500000, @2],
                     @[@600000, @3],
                     @[@800000, @2],
                     @[@1000000, @2],
                     @[@1200000, @2],
                     @[@1600000, @2],
                     @[@2000000, @2],
                     @[@3500000, @2],
                     @[@5000000, @2]];
    
    // @[feet, numberOfBars]
    _imperialTable = @[@[@4, @2],
                       @[@6, @3],
                       @[@10, @2],
                       @[@20, @2],
                       @[@30, @3],
                       @[@50, @2],
                       @[@75, @3],
                       @[@100, @2],
                       @[@200, @2],
                       @[@300, @3],
                       @[@400, @2],
                       @[@600, @3],
                       @[@800, @2],
                       @[@1000, @2],
                       @[@(0.25f*MGLFeetPerMile), @2],
                       @[@(0.5f*MGLFeetPerMile), @2],
                       @[@(1*MGLFeetPerMile), @2],
                       @[@(2*MGLFeetPerMile), @2],
                       @[@(3*MGLFeetPerMile), @3],
                       @[@(4*MGLFeetPerMile), @2],
                       @[@(8*MGLFeetPerMile), @2],
                       @[@(12*MGLFeetPerMile), @2],
                       @[@(15*MGLFeetPerMile), @3],
                       @[@(20*MGLFeetPerMile), @2],
                       @[@(30*MGLFeetPerMile), @3],
                       @[@(40*MGLFeetPerMile), @2],
                       @[@(80*MGLFeetPerMile), @2],
                       @[@(120*MGLFeetPerMile), @2],
                       @[@(200*MGLFeetPerMile), @2],
                       @[@(300*MGLFeetPerMile), @3],
                       @[@(400*MGLFeetPerMile), @2],
                       @[@(800*MGLFeetPerMile), @2],
                       @[@(1000*MGLFeetPerMile), @2],
                       @[@(2000*MGLFeetPerMile), @2],
                       @[@(3000*MGLFeetPerMile), @3],
                       @[@(4000*MGLFeetPerMile), @2]];
}

#pragma mark - Dimensions

- (CGSize)intrinsicContentSize {
    return CGSizeMake(self.actualWidth, 16);
}

- (CGFloat)actualWidth {
    return floorf(self.row.firstObject.floatValue / [self unitsPerPoint]);
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

- (NSArray<NSArray<NSNumber *> *> *)validRows {
    CLLocationDistance minimumDistance = [self minimumWidth] * [self unitsPerPoint];
    CLLocationDistance maximumDistance = [self maximumWidth] * [self unitsPerPoint];
    
    NSMutableArray *validRows = [NSMutableArray array];
    NSArray *table = [self usesMetricSystem] ? self.metricTable : self.imperialTable;
    for (NSArray<NSNumber *> *row in table) {
        CLLocationDistance distance = row.firstObject.floatValue;
        if (distance >= minimumDistance && distance <= maximumDistance) {
            [validRows addObject:row];
        }
    }
    
    return validRows;
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
    
    self.row = [self validRows].lastObject;
    
    [self invalidateIntrinsicContentSize];
    [self setNeedsLayout];
}

- (void)setRow:(NSArray<NSNumber *> *)row {
    if (![row isEqualToArray:_row]) {
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
        for (NSUInteger i = 0; i < self.row.lastObject.integerValue; i++) {
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
        
        for (NSUInteger i = 0; i <= self.row.lastObject.integerValue; i++) {
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
    
    if (!self.row.lastObject || !self.row.lastObject.integerValue) {
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
        CLLocationDistance barDistance = (self.row.firstObject.integerValue / self.row.lastObject.integerValue) * (i + 1);
        
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
