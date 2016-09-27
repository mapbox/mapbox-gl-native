#import <Foundation/Foundation.h>

/**
 A source supplies data to be shown on the map. Sources don't contain styling details like color or width. Use style layers to give visual representation to sources. 
 
 You should use the concrete subclasses of MGLSource to create vector, raster, GeoJSON, and other source types.
 */
@interface MGLSource : NSObject

/**
 A string that uniquely identifies the source.
 */
@property (nonatomic, copy) NSString *sourceIdentifier;

- (instancetype)initWithSourceIdentifier:(NSString *)sourceIdentifier;

@end
