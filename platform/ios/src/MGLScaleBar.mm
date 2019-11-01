#import "Mapbox.h"
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
@property (nonatomic, assign) CGFloat borderWidth;
@property (nonatomic) NSMutableDictionary* labelImageCache;
@property (nonatomic) MGLScaleBarLabel* prototypeLabel;
@property (nonatomic) CGFloat lastLabelWidth;
@property (nonatomic) CGSize size;
@property (nonatomic) BOOL recalculateSize;
@property (nonatomic) BOOL shouldLayoutBars;
@property (nonatomic) NSNumber *testingRightToLeftOverride;
@end

static const CGFloat MGLBarHeight = 4;
static const CGFloat MGLFeetPerMeter = 3.28084;
static const CGFloat MGLScaleBarLabelWidthHint = 30.0;
static const CGFloat MGLScaleBarMinimumBarWidth = 30.0; // Arbitrary

@interface MGLScaleBarLabel : UILabel

@end

@implementation MGLScaleBarLabel

- (void)drawTextInRect:(CGRect)rect {
    CGSize shadowOffset = self.shadowOffset;
    
    CGContextRef context = UIGraphicsGetCurrentContext();
    CGContextSetLineWidth(context, 2);
    CGContextSetLineJoin(context, kCGLineJoinRound);
    
    CGContextSetTextDrawingMode(context, kCGTextStroke);
    self.textColor = [UIColor whiteColor];
    [super drawTextInRect:rect];
    
    CGContextSetTextDrawingMode(context, kCGTextFill);
    self.textColor = [UIColor blackColor];
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
    _size = CGSizeZero;
    
    _primaryColor = [UIColor colorWithRed:18.0/255.0 green:45.0/255.0 blue:17.0/255.0 alpha:1];
    _secondaryColor = [UIColor colorWithRed:247.0/255.0 green:247.0/255.0 blue:247.0/255.0 alpha:1];
    _borderWidth = 1.0f;
    
    self.clipsToBounds = NO;
    self.hidden = YES;
    
    _containerView                     = [[UIView alloc] init];
    _containerView.clipsToBounds       = YES;
    _containerView.backgroundColor     = _secondaryColor;
    _containerView.layer.borderColor   = _primaryColor.CGColor;
    _containerView.layer.borderWidth   = _borderWidth / [[UIScreen mainScreen] scale];

    _containerView.layer.cornerRadius  = MGLBarHeight / 2.0;
    _containerView.layer.masksToBounds = YES;

    [self addSubview:_containerView];
    
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
    _lastLabelWidth = MGLScaleBarLabelWidthHint;

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

- (void)setBorderWidth:(CGFloat)borderWidth {
    _borderWidth = borderWidth;
    _containerView.layer.borderWidth = borderWidth / [[UIScreen mainScreen] scale];
}

// Determines the width of the bars NOT the size of the entire scale bar,
// which includes space for (half) a label.
// Uses the current set `row`
- (CGFloat)actualWidth {
    CGFloat unitsPerPoint = [self unitsPerPoint];
    
    if (unitsPerPoint == 0.0) {
        return 0.0;
    }
        
    CGFloat width = self.row.distance / unitsPerPoint;

    if (width <= MGLScaleBarMinimumBarWidth) {
        return 0.0;
    }

    // Round, so that each bar section has an integer width
    return self.row.numberOfBars * floor(width/self.row.numberOfBars);
}

- (CGFloat)maximumWidth {
    // TODO: Consider taking Scale Bar margins into account here.
    CGFloat fullWidth = CGRectGetWidth(self.superview.bounds);
    return floorf(fullWidth / 2);
}

- (CGFloat)unitsPerPoint {
    return [self usesMetricSystem] ? self.metersPerPoint : self.metersPerPoint * MGLFeetPerMeter;
}

#pragma mark - Convenience methods

- (BOOL)usesRightToLeftLayout {
    if (self.testingRightToLeftOverride) {
        return [self.testingRightToLeftOverride boolValue];
    }

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
    
    self.recalculateSize = YES;
    [self invalidateIntrinsicContentSize];
}

- (CGSize)intrinsicContentSize {
    // Size is calculated elsewhere - since intrinsicContentSize is part of the
    // constraint system, this should be done in updateConstraints
    if (self.size.width < 0.0) {
        return CGSizeZero;
    }
    return self.size;
}

/// updateConstraints
///
/// The primary job of updateConstraints here is to recalculate the
/// intrinsicContentSize: _metersPerPoint and the maximum width determine the
/// current "row", which in turn determines the "actualWidth". To obtain the full
/// width of the scale bar, we also need to include some space for the "last"
/// label

- (void)updateConstraints {
    if (self.isHidden || !self.recalculateSize) {
        [super updateConstraints];
        return;
    }
        
    // TODO: Improve this (and the side-effects)
    self.row = [self preferredRow];

    NSAssert(self.row.numberOfBars > 0, @"");

    CGFloat totalBarWidth = self.actualWidth;
    
    if (totalBarWidth <= 0.0) {
        [super updateConstraints];
        return;
    }

    // Determine the "lastLabelWidth". This has changed to take a maximum of each
    // label, to ensure that the size does not change in LTR & RTL layouts, and
    // also to stop jiggling when the scale bar is on the right hand of the screen
    // This will most likely be a constant, as we take a max using a "hint" for
    // the initial value
    
    if (self.shouldLayoutBars) {
        [self updateLabels];
    }
    
    CGFloat halfLabelWidth = ceil(self.lastLabelWidth/2);
       
    self.size = CGSizeMake(totalBarWidth + halfLabelWidth, 16);
       
    [self setNeedsLayout];
    [super updateConstraints]; // This calls intrinsicContentSize
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
    
    self.shouldLayoutBars = YES;
    
    _row = row;
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
        
        self.lastLabelWidth = MAX(self.lastLabelWidth, image.size.width);

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

    if (!self.recalculateSize) {
        return;
    }

    self.recalculateSize = NO;

    // If size is 0, then we keep the existing layout (which will fade out)
    if (self.size.width <= 0.0) {
        return;
    }

    CGFloat totalBarWidth = self.actualWidth;

    if (totalBarWidth <= 0.0) {
        return;
    }
    
    if (self.shouldLayoutBars) {
        self.shouldLayoutBars = NO;
        [_bars makeObjectsPerformSelector:@selector(removeFromSuperview)];
        _bars = nil;
    }

    // Re-layout the component bars and labels of the scale bar
    CGFloat intrinsicContentHeight = self.intrinsicContentSize.height;
    CGFloat barWidth               = totalBarWidth/self.bars.count;

    BOOL RTL               = [self usesRightToLeftLayout];
    CGFloat halfLabelWidth = ceil(self.lastLabelWidth/2);
    CGFloat barOffset      = RTL ? halfLabelWidth : 0.0;
    
    self.containerView.frame = CGRectMake(barOffset,
                                          intrinsicContentHeight - MGLBarHeight,
                                          totalBarWidth,
                                          MGLBarHeight);

    [self layoutBarsWithWidth:barWidth];
    
    CGFloat yPosition = round(0.5 * ( intrinsicContentHeight - MGLBarHeight));
    CGFloat barDelta = RTL ? -barWidth : barWidth;
    [self layoutLabelsWithOffset:barOffset delta:barDelta yPosition:yPosition];
}

- (void)layoutBarsWithWidth:(CGFloat)barWidth {
    NSUInteger i = 0;
    for (UIView *bar in self.bars) {
        CGFloat xPosition = barWidth * i;
        bar.backgroundColor = (i % 2 == 0) ? self.primaryColor : self.secondaryColor;
        bar.frame = CGRectMake(xPosition, 0, barWidth, MGLBarHeight);
        i++;
    }
}

- (void)layoutLabelsWithOffset:(CGFloat)barOffset delta:(CGFloat)barDelta yPosition:(CGFloat)yPosition {
#if !defined(NS_BLOCK_ASSERTIONS)
    NSUInteger countOfVisibleLabels = 0;
    for (UIView *view in self.labelViews) {
        if (!view.isHidden) {
            countOfVisibleLabels++;
        }
    }
    NSAssert(self.bars.count == countOfVisibleLabels - 1, @"");
#endif
    
    CGFloat xPosition = barOffset;
    
    if (barDelta < 0) {
        xPosition -= (barDelta*self.bars.count);
    }
    
    for (UIView *label in self.labelViews) {
        // Label frames have 0 size - though the layer contents use "center" and do
        // not clip to bounds. This way we don't need to worry about positioning the
        // label. (Though you won't see the label in the view debugger)
        label.frame = CGRectMake(xPosition, yPosition, 0.0, 0.0);
        
        xPosition += barDelta;
    }
}
@end
