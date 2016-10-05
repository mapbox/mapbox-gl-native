#import <Foundation/Foundation.h>

/**
 A source supplies data to be shown on the map. Sources don't contain styling 
 details like color or width. Use subclasses of `MGLStyleLayer` to give visual
 representation to sources.
 
 You should use the concrete subclasses of `MGLSource` to create vector, 
 raster, GeoJSON, and other source types.
 */
@interface MGLSource : NSObject

#pragma mark Initializing a Source

/**
 Returns a source initialized with an identifier.
 
 After initializing and configuring the source, add it to a map view’s style
 using the `-[MGLStyle addSource:]` method.
 
 @param identifier A string that uniquely identifies the source in the style to
    which it is added.
 @return An initialized source.
 */
- (instancetype)initWithIdentifier:(NSString *)identifier;

#pragma mark Identifying a Source

/**
 A string that uniquely identifies the source in the style to which it is added.
 */
@property (nonatomic, copy) NSString *identifier;

@end
