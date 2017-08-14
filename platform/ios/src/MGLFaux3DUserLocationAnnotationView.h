#import <UIKit/UIKit.h>
#import "MGLUserLocationAnnotationView.h"

const CGFloat MGLUserLocationAnnotationDotSize = 22.0;
const CGFloat MGLUserLocationAnnotationHaloSize = 115.0;

const CGFloat MGLUserLocationAnnotationPuckSize = 45.0;
const CGFloat MGLUserLocationAnnotationArrowSize = MGLUserLocationAnnotationPuckSize * 0.6;

// Threshold in radians between heading indicator rotation updates.
const CGFloat MGLUserLocationHeadingUpdateThreshold = 0.01;

@interface MGLFaux3DUserLocationAnnotationView : MGLUserLocationAnnotationView

@end
