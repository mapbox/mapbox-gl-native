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
@property (nonatomic) NSArray<UIView *> *labelViews;
@property (nonatomic) NSArray<UIView *> *bars;
@property (nonatomic) UIView *containerView;
@property (nonatomic) MGLDistanceFormatter *formatter;
@property (nonatomic, assign) MGLRow row;
@property (nonatomic) UIColor *primaryColor;
@property (nonatomic) UIColor *secondaryColor;
@property (nonatomic) CALayer *borderLayer;
@property (nonatomic, assign) CGFloat borderWidth;
@property (nonatomic) NSMutableDictionary* labelImageCache;
@property (nonatomic) MGLScaleBarLabel* prototypeLabel;
@property (nonatomic) CGFloat lastLabelWidth;

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

    // Image labels are now images
    _labelImageCache              = [[NSMutableDictionary alloc] init];
    _prototypeLabel               = [[MGLScaleBarLabel alloc] init];
    _prototypeLabel.font          = [UIFont systemFontOfSize:8 weight:UIFontWeightMedium];
    _prototypeLabel.clipsToBounds = NO;

    NSUInteger numberOfLabels = 4;
    NSMutableArray *labelViews = [NSMutableArray arrayWithCapacity:numberOfLabels];

    for (NSUInteger i = 0; i < numberOfLabels; i++) {
        UIView *view = [[UIView alloc] init];
        view.bounds        = CGRectZero;
        view.clipsToBounds = NO;
        view.contentMode   = UIViewContentModeCenter;
        view.hidden        = YES;
        [labelViews addObject:view];
        [self addSubview:view];
    }
    _labelViews = [labelViews copy];

    // Zero is a special case (no formatting)
    [self addZeroLabel];

    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(resetLabelImageCache) name:NSCurrentLocaleDidChangeNotification object:nil];
}

- (void)dealloc {
    [[NSNotificationCenter defaultCenter] removeObserver:self];
}

- (void)resetLabelImageCache {
    self.labelImageCache = [[NSMutableDictionary alloc] init];
    [self addZeroLabel];
}

#pragma mark - Dimensions

- (CGSize)intrinsicContentSize {
    return self.actualWidth > 0 ? CGSizeMake(ceil(self.actualWidth + self.lastLabelWidth/2), 16) : CGSizeZero;
}

- (CGFloat)actualWidth {
    CGFloat width = self.row.distance / [self unitsPerPoint];
    return !isnan(width) ? width : 0;
}

- (CGFloat)maximumWidth {
    CGFloat fullWidth = CGRectGetWidth(self.superview.bounds);
    return floorf(fullWidth / 2);
}

- (CGFloat)unitsPerPoint {
    return [self usesMetricSystem] ? self.metersPerPoint : self.metersPerPoint * MGLFeetPerMeter;
}

#pragma mark - Convenience methods

- (BOOL)usesRightToLeftLayout {
    return [UIView userInterfaceLayoutDirectionForSemanticContentAttribute:self.superview.semanticContentAttribute] == UIUserInterfaceLayoutDirectionRightToLeft;
}

- (BOOL)usesMetricSystem {
    NSLocale *locale = [NSLocale currentLocale];
    return [[locale objectForKey:NSLocaleUsesMetricSystem] boolValue];
}

- (MGLRow)preferredRow {
    CLLocationDistance maximumDistance = [self maximumWidth] * [self unitsPerPoint];
    
    BOOL useMetric = [self usesMetricSystem];

    const MGLRow *row;
    const MGLRow *table;
    NSUInteger count;

    if (useMetric) {
        row = table = MGLMetricTable;
        count = sizeof(MGLMetricTable) / sizeof(MGLMetricTable[0]);
    }
    else {
        row = table = MGLImperialTable;
        count = sizeof(MGLImperialTable) / sizeof(MGLImperialTable[0]);
    }

    while (row < table + count) {
        if (row->distance > maximumDistance) {
            // use the previous row
            NSAssert(row != table, @"");
            return *(row - 1);
        }
        ++row;
    }

    // Didn't find it, just return the first.
    return *table;
}

#pragma mark - Setters

- (void)setMetersPerPoint:(CLLocationDistance)metersPerPoint {
    if (_metersPerPoint == metersPerPoint) {
        return;
    }
    
    _metersPerPoint = metersPerPoint;
    
    [self updateVisibility];
    
    self.row = [self preferredRow];

    [self invalidateIntrinsicContentSize];
    [self setNeedsLayout];
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
    _bars = nil;

    [self updateLabels];
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

#pragma mark - Labels

- (void)addZeroLabel {
    NSDecimalNumber *zeroNumber = [NSDecimalNumber decimalNumberWithString:@"0"];
    NSNumberFormatter *formatter = [[NSNumberFormatter alloc] init];
    NSString *text = [formatter stringFromNumber:zeroNumber];

    UIImage* image = [self imageForLabelText:text];
    [self.labelImageCache setObject:image forKey:@(0)];
}

- (UIImage*)imageForLabelText:(NSString*)text {
    self.prototypeLabel.text = text;
    [self.prototypeLabel setNeedsDisplay];
    [self.prototypeLabel sizeToFit];

    // Now render
    UIGraphicsBeginImageContextWithOptions(self.prototypeLabel.bounds.size, NO, 0.0);
    [self.prototypeLabel.layer renderInContext: UIGraphicsGetCurrentContext()];
    UIImage *image = UIGraphicsGetImageFromCurrentImageContext();
    UIGraphicsEndImageContext();
    return image;
}

- (UIImage*)cachedLabelImageForDistance:(CLLocationDistance)barDistance {
    // Make a slightly nicer key, rather than something that's a double.
    NSUInteger floorDist = (NSUInteger)(barDistance*100);

    NSNumber *key = @(floorDist);
    UIImage *cachedImage = [self.labelImageCache objectForKey:key];

    if (cachedImage) {
        return cachedImage;
    }

    // Calc it
    NSString *text = [self.formatter stringFromDistance:barDistance];
    UIImage *image = [self imageForLabelText:text];

    [self.labelImageCache setObject:image forKey:key];

    return image;
}

- (void)updateLabels {
    NSEnumerator<UIView*> *viewEnumerator = [self.labelViews objectEnumerator];
    NSUInteger i = 0;
    CLLocationDistance multiplier = (self.row.distance / self.row.numberOfBars);

    if (![self usesMetricSystem]) {
        multiplier /= MGLFeetPerMeter;
    }

    for (; i <= self.row.numberOfBars; i++) {
        UIView *labelView = [viewEnumerator nextObject];
        labelView.hidden = NO;

        CLLocationDistance barDistance = multiplier * i;
        UIImage *image = [self cachedLabelImageForDistance:barDistance];
        if (i == self.row.numberOfBars) {
            self.lastLabelWidth = image.size.width;
        }
        labelView.layer.contents      = (id)image.CGImage;
        labelView.layer.contentsScale = image.scale;
    }

    // Hide the rest.
    for (; i < self.labelViews.count; i++) {
        UIView *labelView = [viewEnumerator nextObject];
        labelView.hidden = YES;
    }
}

#pragma mark - Layout

- (void)layoutSubviews {
    [super layoutSubviews];

    if (!self.row.numberOfBars) {
        // Current distance is not within allowed range
        return;
    }

    [self layoutBars];
    [self layoutLabels];
}

- (void)layoutBars {
    CGFloat barWidth = round((self.intrinsicContentSize.width - self.borderWidth * 2.0f) / self.bars.count);
    
    NSUInteger i = 0;
    for (UIView *bar in self.bars) {
        CGFloat xPosition = barWidth * i + self.borderWidth;
        bar.backgroundColor = (i % 2 == 0) ? self.primaryColor : self.secondaryColor;
        bar.frame = CGRectMake(xPosition, self.borderWidth, barWidth, MGLBarHeight);
        i++;
    }
    
    self.containerView.frame = CGRectMake(CGRectGetMinX(self.bars.firstObject.frame),
                                          self.intrinsicContentSize.height-MGLBarHeight,
                                          self.actualWidth,
                                          MGLBarHeight+self.borderWidth*2);
    
    [CATransaction begin];
    [CATransaction setDisableActions:YES];
    self.borderLayer.frame = CGRectInset(self.containerView.bounds, self.borderWidth, self.borderWidth);
    self.borderLayer.zPosition = FLT_MAX;
    [CATransaction commit];
}

- (void)layoutLabels {
    CGFloat barWidth = round(self.actualWidth / self.bars.count);
    BOOL RTL = [self usesRightToLeftLayout];
    NSUInteger i = RTL ? self.bars.count : 0;
    for (UIView *label in self.labelViews) {
        CGFloat xPosition = round(barWidth * i - CGRectGetMidX(label.bounds) + self.borderWidth);
        CGFloat yPosition = round(0.5 * (self.intrinsicContentSize.height - MGLBarHeight));

        CGRect frame = label.frame;
        frame.origin.x = xPosition;
        frame.origin.y = yPosition;
        label.frame = frame;

        i = RTL ? i-1 : i+1;
    }
}

@end
