#import "MGLAnnotationImage.h"

NS_ASSUME_NONNULL_BEGIN

@protocol MGLAnnotationImageDelegate <NSObject>

@required
- (void)annotationImageNeedsRedisplay:(MGLAnnotationImage *)annotationImage;

@end

@interface MGLAnnotationImage (Private)

@property (nonatomic, weak) id<MGLAnnotationImageDelegate> delegate;

@end

NS_ASSUME_NONNULL_END
