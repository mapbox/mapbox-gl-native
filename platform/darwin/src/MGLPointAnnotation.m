#import "MGLPointAnnotation.h"

@implementation MGLPointAnnotation

@synthesize coordinate;
@synthesize canShowCallout;
@synthesize calloutView;

- (NSString *)description
{
    return [NSString stringWithFormat:@"<%@: %p; title = %@; subtitle = %@; coordinate = %f, %f; callout: %@ (%@)>",
            NSStringFromClass([self class]), (void *)self,
            self.title ? [NSString stringWithFormat:@"\"%@\"", self.title] : self.title,
            self.subtitle ? [NSString stringWithFormat:@"\"%@\"", self.subtitle] : self.subtitle,
            coordinate.latitude, coordinate.longitude,
            canShowCallout ? @"yes" : @"no", calloutView];
}

@end
