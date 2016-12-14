#import "MGLScaleControlView.h"

@interface MGLScaleControlView()
@property (nonatomic, assign) double zoomLevel;
@property (nonatomic, assign) CLLocationDistance metersPerPoint;
@property (nonatomic) NSArray *labels;
@property (nonatomic) NSArray *bars;
@property (nonatomic) UILabel *debugLabel;
@end

@implementation MGLScaleControlView

- (void)updateWithZoomLevel:(double)zoomLevel metersPerPoint:(CLLocationDistance)metersPerPoint {
    self.zoomLevel = zoomLevel;
    self.metersPerPoint = metersPerPoint;
}

- (void)setZoomLevel:(double)zoomLevel {
    _zoomLevel = zoomLevel;
    [self.debugLabel setText:[NSString stringWithFormat:@"%0.2f", zoomLevel]];
    [self.debugLabel sizeToFit];
    [self setNeedsLayout];
}

- (CGSize)intrinsicContentSize {
    return CGSizeMake(100, 10);
}

- (UILabel *)debugLabel {
    if (!_debugLabel) {
        _debugLabel = [[UILabel alloc] init];
        _debugLabel.font = [UIFont systemFontOfSize:8];
        [self addSubview:_debugLabel];
    }
    return _debugLabel;
}

- (void)layoutSubviews {
    [super layoutSubviews];
    
    self.debugLabel.frame = CGRectMake(CGRectGetMidX(self.bounds)-CGRectGetMidX(_debugLabel.bounds),
                                       CGRectGetMaxY(self.bounds),
                                       CGRectGetWidth(_debugLabel.frame),
                                       CGRectGetHeight(_debugLabel.frame));
    self.backgroundColor = [UIColor blackColor];
}

@end
