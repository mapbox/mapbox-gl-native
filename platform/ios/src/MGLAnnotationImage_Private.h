#import "MGLAnnotationImage.h"

NS_ASSUME_NONNULL_BEGIN

@protocol MGLAnnotationImageDelegate <NSObject>

@required
- (void)annotationImageNeedsRedisplay:(MGLAnnotationImage *)annotationImage;

@end

@interface MGLAnnotationImage (Private)

/// Unique identifier of the sprite image used by the style to represent the receiver’s `image`.
@property (nonatomic, strong, nullable) NSString *styleIconIdentifier;

@property (nonatomic, weak) id<MGLAnnotationImageDelegate> delegate;

@end

NS_ASSUME_NONNULL_END
