#import "MGLCompassCell.h"

@implementation MGLCompassCell

- (instancetype)init {
    if (self = [super init]) {
        self.sliderType = NSCircularSlider;
        // A tick mark for each cardinal direction.
        self.numberOfTickMarks = 4;
        // This slider goes backwards!
        self.minValue = -360;
        self.maxValue = 0;
    }
    return self;
}

- (void)drawKnob:(NSRect)knobRect {
    // Draw a red triangle pointing whichever way the slider is facing.
    NSBezierPath *trianglePath = [NSBezierPath bezierPath];
    [trianglePath moveToPoint:NSMakePoint(NSMinX(knobRect), NSMaxY(knobRect))];
    [trianglePath lineToPoint:NSMakePoint(NSMaxX(knobRect), NSMaxY(knobRect))];
    [trianglePath lineToPoint:NSMakePoint(NSMidX(knobRect), NSMinY(knobRect))];
    [trianglePath closePath];
    NSAffineTransform *transform = [NSAffineTransform transform];
    [transform translateXBy:NSMidX(knobRect) yBy:NSMidY(knobRect)];
    [transform scaleBy:0.8];
    [transform rotateByDegrees:self.doubleValue];
    [transform translateXBy:-NSMidX(knobRect) yBy:-NSMidY(knobRect)];
    [trianglePath transformUsingAffineTransform:transform];
    [[NSColor redColor] setFill];
    [trianglePath fill];
}

@end
