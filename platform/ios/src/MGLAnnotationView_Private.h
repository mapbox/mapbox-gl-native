#import "MGLAnnotationView.h"
#import "MGLAnnotation.h"

NS_ASSUME_NONNULL_BEGIN

@interface MGLAnnotationView (Private)

@property (nonatomic) id<MGLAnnotation> annotation;
@property (nonatomic, readwrite, nullable) NSString *reuseIdentifier;

@end

NS_ASSUME_NONNULL_END
