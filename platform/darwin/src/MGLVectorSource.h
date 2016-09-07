#import "MGLSource.h"

/**
 A vector tile source. Tiles must be in Mapbox Vector Tile format.
 
 @see <a href="https://www.mapbox.com/mapbox-gl-style-spec/#sources-vector">the style specification</a>
 */
@interface MGLVectorSource : MGLSource

@property (nonatomic, readonly, copy) NSURL *URL;

/**
 Initializes and returns a vector source from a remote url.
 
 @param sourceIdentifier The source identifier.
 @param URL A remote URL holding the vector source data.
 
 @return An `MGLVectorSource`.
 */
- (instancetype)initWithSourceIdentifier:(NSString *)sourceIdentifier URL:(NSURL *)url;

@end
