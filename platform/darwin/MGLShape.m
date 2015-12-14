#import "MGLShape.h"

@implementation MGLShape

- (CLLocationCoordinate2D)coordinate
{
    [[NSException exceptionWithName:@"MGLAbstractClassException"
                             reason:@"MGLShape is an abstract class"
                           userInfo:nil] raise];

    return CLLocationCoordinate2DMake(MAXFLOAT, MAXFLOAT);
}

@end
