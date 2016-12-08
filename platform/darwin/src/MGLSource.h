#import <Foundation/Foundation.h>

/**
 `MGLSource` is an abstract base class for map content sources. A map content
 source supplies content to be shown on the map. A source is added to an
 `MGLStyle` object along with an `MGLForegroundStyleLayer` object. The
 foreground style layer defines the appearance of any content supplied by the
 source.
 
 Each source defined by the style JSON file is represented at runtime by an
 `MGLSource` object that you can use to refine the map’s content. You can also
 add and remove sources dynamically using methods such as
 `-[MGLStyle addSource:]` and `-[MGLStyle sourceWithIdentifier:]`.
 
 Do not create instances of this class directly, and do not create your own
 subclasses of this class. Instead, create instances of `MGLRasterSource`,
 `MGLShapeSource`, and `MGLVectorSource`.
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
