#import "DroppedPinAnnotation.h"

#import "LocationCoordinate2DTransformer.h"
#import "TimeIntervalTransformer.h"

#import <Mapbox/Mapbox.h>

static MGLCoordinateFormatter *DroppedPinCoordinateFormatter;

@implementation DroppedPinAnnotation {
    NSTimer *_timer;
    NSTimeInterval _priorShownTimeInterval;
    NSDate *_dateShown;

    NSValueTransformer *_timeIntervalTransformer;
}

+ (void)initialize {
    if (self == [DroppedPinAnnotation class]) {
        DroppedPinCoordinateFormatter = [[MGLCoordinateFormatter alloc] init];
    }
}

- (instancetype)init {
    if (self = [super init]) {
        _timeIntervalTransformer = [NSValueTransformer valueTransformerForName:
                                    NSStringFromClass([TimeIntervalTransformer class])];
        [self update:nil];
    }
    return self;
}

- (void)dealloc {
    [self pause];
}

- (void)setCoordinate:(CLLocationCoordinate2D)coordinate {
    super.coordinate = coordinate;
    [self update:nil];
}

- (NSTimeInterval)elapsedShownTime {
    return _priorShownTimeInterval - _dateShown.timeIntervalSinceNow;
}

- (void)resume {
    _dateShown = [NSDate date];
    _timer = [NSTimer scheduledTimerWithTimeInterval:1
                                              target:self
                                            selector:@selector(update:)
                                            userInfo:nil
                                             repeats:YES];
}

- (void)pause {
    [_timer invalidate];
    _timer = nil;
    _priorShownTimeInterval -= _dateShown.timeIntervalSinceNow;
    _dateShown = nil;
}

- (void)update:(NSTimer *)timer {
    NSString *coordinate = [DroppedPinCoordinateFormatter stringFromCoordinate:self.coordinate];
    NSString *elapsedTime = [_timeIntervalTransformer transformedValue:@(self.elapsedShownTime)];
    self.subtitle = [NSString stringWithFormat:@"%@\nSelected for %@", coordinate, elapsedTime];
}

@end
