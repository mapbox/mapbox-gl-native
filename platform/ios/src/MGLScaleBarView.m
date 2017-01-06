#import <Mapbox/Mapbox.h>

#import "MGLScaleBarView_Private.h"

@interface MGLScaleBarView()
@property (nonatomic, assign, getter=isVisible) BOOL visible;
@property (nonatomic, assign) CLLocationDistance metersPerPoint;
@property (nonatomic) NSArray<UILabel *> *labels;
@property (nonatomic) NSArray<UIView *> *bars;
@property (nonatomic) UIView *containerView;
@property (nonatomic) MGLDistanceFormatter *formatter;
@property (nonatomic) NSArray<NSArray<NSNumber*>*> *metricTable;
@property (nonatomic) NSArray<NSNumber*> *row;
@property (nonatomic) UIColor *primaryColor;
@property (nonatomic) UIColor *secondaryColor;
@property (nonatomic) CALayer *borderLayer;
@end

static const CGFloat BAR_HEIGHT = 4;

@implementation MGLScaleBarView

- (instancetype)initWithCoder:(NSCoder *)aDecoder {
    if (self = [super initWithCoder:aDecoder]) {
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
    self.clipsToBounds = NO;
    self.alpha = 0;
    
    _visible = NO;
    _primaryColor = [UIColor colorWithRed:62.0/255.0 green:62.0/255.0 blue:62.0/255.0 alpha:1];
    _secondaryColor = [UIColor colorWithRed:247.0/255.0 green:247.0/255.0 blue:247.0/255.0 alpha:1];
    
    _containerView = [[UIView alloc] init];
    _containerView.clipsToBounds = YES;
    [self addSubview:_containerView];
    
    _borderLayer = [CAShapeLayer layer];
    _borderLayer.borderColor = [UIColor colorWithRed:177.0/255.0 green:177.0/255.0 blue:177.0/255.0 alpha:1].CGColor;
    _borderLayer.borderWidth = 1.0 / [[UIScreen mainScreen] scale];
    
    [_containerView.layer addSublayer:_borderLayer];
    
    _formatter = [[MGLDistanceFormatter alloc] init];
    _row = @[@0, @0];
    _metricTable = @[@[@4, @2],
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
}

#pragma mark - Dimensions

- (CGSize)intrinsicContentSize {
    return CGSizeMake(self.actualWidth, 16);
}

- (CGFloat)preferredWidth {
    CGFloat padding = CGRectGetMinX(self.frame);
    return ([self maximumWidth] - padding) / 2;
}

- (CGFloat)actualWidth {
    return floorf(self.row.firstObject.floatValue / self.metersPerPoint);
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

#pragma mark - Convenient methods

- (NSArray<NSArray<NSNumber*>*> *)validRows {
    CLLocationDistance minimumDistance = [self minimumWidth] * self.metersPerPoint;
    CLLocationDistance maximumDistance = [self maximumWidth] * self.metersPerPoint;
    
    NSMutableArray *validRows = [NSMutableArray array];
    for (NSArray<NSNumber *> *row in self.metricTable) {
        CLLocationDistance distance = row.firstObject.floatValue;
        if (distance >= minimumDistance && distance <= maximumDistance) {
            [validRows addObject:row];
        }
    }
    
    return validRows;
}

// Find the row closest to -[MGLScaleBarView preferredWidth]
- (NSArray<NSNumber*> *)preferredRowInRows:(NSArray<NSArray<NSNumber*>*> *)rows {
    CLLocationDistance diff = CGFLOAT_MAX;
    CLLocationDistance preferredDistance = [self preferredWidth] * self.metersPerPoint;
    NSArray<NSNumber *> *preferredRow = rows.firstObject;
    for (NSArray<NSNumber *> *row in rows) {
        CLLocationDistance currentDiff = ABS(preferredDistance - row.firstObject.floatValue);
        if (currentDiff < diff) {
            preferredRow = row;
        }
    }
    return preferredRow;
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
    self.row = [self preferredRowInRows:[self validRows]];
    
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
        NSMutableArray *labels = [NSMutableArray array];
        for (NSUInteger i = 0; i <= self.row.lastObject.integerValue; i++) {
            UILabel *label = [[UILabel alloc] init];
            label.text = @"0";
            label.font = [UIFont systemFontOfSize:8];
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
    
    CGFloat barWidth = floorf(self.bounds.size.width / self.bars.count);
    
    [self.bars enumerateObjectsUsingBlock:^(UIView * _Nonnull bar, NSUInteger idx, BOOL * _Nonnull stop) {
        CGFloat xPos = barWidth * idx;
        bar.backgroundColor = (idx % 2 == 0) ? self.primaryColor : self.secondaryColor;
        bar.frame = CGRectMake(xPos, 0, barWidth, BAR_HEIGHT);
    }];
    
    self.containerView.backgroundColor = self.bars.lastObject.backgroundColor;
    self.containerView.frame = CGRectMake(CGRectGetMinX(self.bars.firstObject.frame),
                                          CGRectGetMaxY(self.bounds)-BAR_HEIGHT,
                                          self.actualWidth,
                                          BAR_HEIGHT);
    
    self.containerView.layer.cornerRadius = CGRectGetMidY(self.containerView.bounds);
    
    [CATransaction begin];
    [CATransaction setDisableActions:YES];
    self.borderLayer.frame = self.containerView.bounds;
    self.borderLayer.zPosition = FLT_MAX;
    [CATransaction commit];
}

- (void)layoutLabels {
    CGFloat barWidth = floorf(self.bounds.size.width / self.bars.count);
    [self.labels enumerateObjectsUsingBlock:^(UILabel * _Nonnull label, NSUInteger idx, BOOL * _Nonnull stop) {
        CGFloat xPosition = barWidth * idx - CGRectGetMidX(label.bounds);
        label.frame = CGRectMake(xPosition, 0,
                                 CGRectGetWidth(label.bounds),
                                 CGRectGetHeight(label.bounds));
    }];
}

- (void)updateLabels {
    NSArray *labels = [self.labels subarrayWithRange:NSMakeRange(1, self.labels.count-1)];
    [labels enumerateObjectsUsingBlock:^(UILabel * _Nonnull label, NSUInteger idx, BOOL * _Nonnull stop) {
        CLLocationDistance barDistance = (self.row.firstObject.integerValue / self.row.lastObject.integerValue) * (idx + 1);
        label.text = [self.formatter stringFromDistance:barDistance];
        [label sizeToFit];
    }];
}

@end
