#import "MGLScaleControlView.h"

@interface MGLScaleControlView()
@property (nonatomic, assign) double zoomLevel;
@property (nonatomic, assign) CLLocationDistance metersPerPoint;
@property (nonatomic) NSArray<UILabel *> *labels;
@property (nonatomic) NSArray<UIView *> *bars;
@property (nonatomic) UILabel *debugLabel;
@property (nonatomic) NSLengthFormatter *formatter;
@end

@implementation MGLScaleControlView

static const NSUInteger TEMP_NUMBER_OF_BARS = 2;
static const CGFloat BASE_WIDTH = 100;

- (instancetype)init {
    if (self == [super init]) {
        _formatter = [[NSLengthFormatter alloc] init];
        _formatter.unitStyle = NSDateComponentsFormatterUnitsStyleAbbreviated;
    }
    return self;
}

- (void)updateWithZoomLevel:(double)zoomLevel metersPerPoint:(CLLocationDistance)metersPerPoint {
    self.zoomLevel = zoomLevel;
    self.metersPerPoint = metersPerPoint;
}

- (void)setZoomLevel:(double)zoomLevel {
    _zoomLevel = zoomLevel;
    [self.debugLabel setText:[NSString stringWithFormat:@"%0.2f", zoomLevel]];
    [self.debugLabel sizeToFit];
    [self invalidateIntrinsicContentSize];
    [self setNeedsLayout];
}

- (NSUInteger)stepsForDistance:(CLLocationDistance)distance {
    if (distance <= 15) {
        return 5;
    } else if (distance <= 20) {
        return 12;
    } else if (distance < 100) {
        return 25;
    } else if (distance < 250) {
        return 50;
    }
    
    return 100;
}

- (NSUInteger)nearestStepForDistance:(CLLocationDistance)distance {
    NSUInteger step = [self stepsForDistance:distance];
    return step * floor((distance/step)+0.5);
}

- (CGSize)intrinsicContentSize {
    return CGSizeMake(self.preferredWidth, 10);
}

- (CGFloat)preferredWidth {
    CGFloat widthInMeters = BASE_WIDTH * self.metersPerPoint;
    NSUInteger step = [self nearestStepForDistance:widthInMeters];
    return step / self.metersPerPoint;
}

- (NSArray<UIView *> *)bars {
    if (!_bars) {
        NSMutableArray *bars = [NSMutableArray array];
        for (NSUInteger i = 0; i < TEMP_NUMBER_OF_BARS; i++) {
            UIView *bar = [[UIView alloc] init];
            [bars addObject:bar];
            [self addSubview:bar];
        }
        _bars = bars;
    }
    return _bars;
}

- (NSArray<UILabel *> *)labels {
    if (!_labels) {
        NSMutableArray *labels = [NSMutableArray array];
        for (NSUInteger i = 0; i < TEMP_NUMBER_OF_BARS+1; i++) {
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

- (UILabel *)debugLabel {
    if (!_debugLabel) {
        _debugLabel = [[UILabel alloc] init];
        _debugLabel.font = [UIFont systemFontOfSize:8];
        _debugLabel.textColor = [UIColor redColor];
        [self addSubview:_debugLabel];
    }
    return _debugLabel;
}

- (void)layoutSubviews {
    [super layoutSubviews];
    
    self.clipsToBounds = NO;
    self.backgroundColor = [UIColor yellowColor];
    
    self.debugLabel.frame = CGRectMake(CGRectGetMidX(self.bounds)-CGRectGetMidX(_debugLabel.bounds),
                                       CGRectGetMaxY(self.bounds),
                                       CGRectGetWidth(_debugLabel.frame),
                                       CGRectGetHeight(_debugLabel.frame));
    
    [self updateLabels];
    [self layoutBars];
    [self layoutLabels];
}

- (void)layoutBars {
    CGFloat barWidth = floorf(self.bounds.size.width / self.bars.count);
    [self.bars enumerateObjectsUsingBlock:^(UIView * _Nonnull bar, NSUInteger idx, BOOL * _Nonnull stop) {
        CGFloat xPos = barWidth * idx;
        bar.backgroundColor = (idx % 2 == 0) ? [UIColor blackColor] : [UIColor whiteColor];
        bar.frame = CGRectMake(xPos, CGRectGetMidY(self.bounds), barWidth, CGRectGetMidY(self.bounds));
    }];
}

- (void)layoutLabels {
    CGFloat barWidth = floorf(self.bounds.size.width / self.bars.count);
    [self.labels enumerateObjectsUsingBlock:^(UILabel * _Nonnull label, NSUInteger idx, BOOL * _Nonnull stop) {
        CGFloat xPosition = barWidth * idx - CGRectGetMidX(label.bounds);
        label.frame = CGRectMake(xPosition,
                                 CGRectGetMidY(self.bounds)-CGRectGetHeight(label.bounds),
                                 CGRectGetWidth(label.bounds),
                                 CGRectGetHeight(label.bounds));
    }];
}

- (void)updateLabels {
    NSArray *labels = [self.labels subarrayWithRange:NSMakeRange(1, self.labels.count-1)];
    [labels enumerateObjectsUsingBlock:^(UILabel * _Nonnull label, NSUInteger idx, BOOL * _Nonnull stop) {
        UIView *bar = self.bars[idx];
        CGFloat scaleWidth = CGRectGetMaxX(bar.frame);
        CGFloat widthInMeters = scaleWidth * self.metersPerPoint;
        NSUInteger distance = [self nearestStepForDistance:widthInMeters];
        label.text = [self.formatter stringFromMeters:distance];
        [label sizeToFit];
    }];
}

@end
