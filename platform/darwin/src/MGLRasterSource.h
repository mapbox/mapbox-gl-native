#import "MGLSource.h"
#import "MGLTypes.h"

@class MGLTileSet;

NS_ASSUME_NONNULL_BEGIN

@interface MGLRasterSource : MGLSource

@property (nonatomic, readonly, copy) NSURL *URL;
@property (nonatomic, readonly, assign) CGFloat tileSize;
@property (nonatomic, readonly, nullable) MGLTileSet *tileSet;

- (instancetype)initWithSourceIdentifier:(NSString *)sourceIdentifier URL:(NSURL *)url tileSize:(CGFloat)tileSize;

- (instancetype)initWithSourceIdentifier:(NSString *)sourceIdentifier tileSize:(CGFloat)tileSize tileSet:(MGLTileSet *)tileSet;

@end

NS_ASSUME_NONNULL_END
