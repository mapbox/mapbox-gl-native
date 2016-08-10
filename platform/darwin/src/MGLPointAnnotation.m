#import "MGLPointAnnotation.h"

@implementation MGLPointAnnotation

@synthesize coordinate;

#if TARGET_OS_IPHONE
@synthesize canShowCallout;
@synthesize calloutView;
#endif

- (NSString *)description
{
    return [NSString stringWithFormat:@"<%@: %p; title = %@; subtitle = %@; coordinate = %f, %f; callout: %@ (%@)>",
            NSStringFromClass([self class]), (void *)self,
            self.title ? [NSString stringWithFormat:@"\"%@\"", self.title] : self.title,
            self.subtitle ? [NSString stringWithFormat:@"\"%@\"", self.subtitle] : self.subtitle,
            coordinate.latitude, coordinate.longitude
#if TARGET_OS_IPHONE
            , canShowCallout ? @"yes" : @"no", calloutView
#endif
            ];
}

@end
