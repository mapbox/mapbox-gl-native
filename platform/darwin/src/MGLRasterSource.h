#import "MGLSource.h"
#import "MGLTypes.h"

NS_ASSUME_NONNULL_BEGIN

@interface MGLRasterSource : MGLSource

@property (nonatomic, readonly, copy) NSURL *URL;
@property (nonatomic, readonly, assign) CGFloat tileSize;
@property (nonatomic, readonly, copy) NS_ARRAY_OF(NSString *) *tileURLTemplates;
@property (nonatomic, readonly) uint8_t minimumZoomLevel;
@property (nonatomic, readonly) uint8_t maximumZoomLevel;

- (instancetype)initWithSourceIdentifier:(NSString *)sourceIdentifier URL:(NSURL *)url tileSize:(CGFloat)tileSize;

- (instancetype)initWithSourceIdentifier:(NSString *)sourceIdentifier tileURLTemplates:(NS_ARRAY_OF(NSString *) *)tileURLTemplates minimumZoomLevel:(NSUInteger)minimumZoomLevel maximumZoomLevel:(NSUInteger)maximumZoomLevel;
@end

NS_ASSUME_NONNULL_END
