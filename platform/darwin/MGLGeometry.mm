#import "MGLGeometry_Private.h"

const MGLCoordinateSpan MGLCoordinateSpanZero = {0, 0};

CGRect MGLExtendRect(CGRect rect, CGPoint point) {
    if (point.x < rect.origin.x) {
        rect.size.width += rect.origin.x - point.x;
        rect.origin.x = point.x;
    }
    if (point.x > rect.origin.x + rect.size.width) {
        rect.size.width += point.x - (rect.origin.x + rect.size.width);
    }
    if (point.y < rect.origin.y) {
        rect.size.height += rect.origin.y - point.y;
        rect.origin.y = point.y;
    }
    if (point.y > rect.origin.y + rect.size.height) {
        rect.size.height += point.y - (rect.origin.y + rect.size.height);
    }
    return rect;
}
