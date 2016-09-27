#import <Foundation/Foundation.h>

/**
 A source supplies data to be shown on the map. Sources don't contain styling 
 details like color or width. Use subclasses of `MGLBaseStyleLayer` to give 
 visual representation to sources.
 
 You should use the concrete subclasses of `MGLSource` to create vector, 
 raster, GeoJSON, and other source types.
 */
@interface MGLSource : NSObject

/**
 A string that uniquely identifies the source.
 */
@property (nonatomic, copy) NSString *sourceIdentifier;

/**
 Initializes a source with the given identifier.

 @param sourceIdentifier A string that uniquely identifies the source.
 */
- (instancetype)initWithSourceIdentifier:(NSString *)sourceIdentifier;

@end
