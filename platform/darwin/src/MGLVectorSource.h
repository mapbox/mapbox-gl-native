#import "MGLSource.h"
#import "MGLTypes.h"

NS_ASSUME_NONNULL_BEGIN

/**
 A vector tile source. Tiles must be in Mapbox Vector Tile format.
 
 @see <a href="https://www.mapbox.com/mapbox-gl-style-spec/#sources-vector">The
    style specification.</a>
 */
@interface MGLVectorSource : MGLSource

@property (nonatomic, readonly, copy) NSURL *URL;

@property (nonatomic, readonly, copy) NS_ARRAY_OF(NSString *) *tileURLTemplates;
@property (nonatomic, readonly) NSUInteger minimumZoomLevel;
@property (nonatomic, readonly) NSUInteger maximumZoomLevel;

/**
 Initializes and returns a vector source from a remote url.
 
 @param sourceIdentifier The source identifier.
 @param URL A remote URL holding the vector source data.
 
 @return An `MGLVectorSource`.
 */
- (instancetype)initWithSourceIdentifier:(NSString *)sourceIdentifier URL:(NSURL *)url;

- (instancetype)initWithSourceIdentifier:(NSString *)sourceIdentifier tileURLTemplates:(NS_ARRAY_OF(NSString *) *)tileURLTemplates minimumZoomLevel:(NSUInteger)minimumZoomLevel maximumZoomLevel:(NSUInteger)maximumZoomLevel;

@end

NS_ASSUME_NONNULL_END
