#import <Foundation/Foundation.h>

#import "MGLFoundation.h"
#import "MGLStyleLayer.h"

NS_ASSUME_NONNULL_BEGIN

@class MGLSource;

/**
 `MGLForegroundStyleLayer` is an abstract superclass for style layers whose
 content is defined by an `MGLSource` object.

 Do not create instances of this class directly, and do not create your own
 subclasses of this class. Instead, create instances of `MGLRasterStyleLayer`
 and the concrete subclasses of `MGLVectorStyleLayer`.
 */
MGL_EXPORT
@interface MGLForegroundStyleLayer : MGLStyleLayer

#pragma mark Initializing a Style Layer

- (instancetype)init __attribute__((unavailable("Use -initWithIdentifier:source: instead.")));
- (instancetype)initWithIdentifier:(NSString *)identifier __attribute__((unavailable("Use -initWithIdentifier:source: instead.")));

/**
 Returns a foreground style layer initialized with an identifier and source.

 After initializing and configuring the style layer, add it to a map view’s
 style using the `-[MGLStyle addLayer:]` or
 `-[MGLStyle insertLayer:belowLayer:]` method.

 @param identifier A string that uniquely identifies the source in the style to
    which it is added.
 @param source The source from which to obtain the data to style. If the source
    has not yet been added to the current style, the behavior is undefined.
 @return An initialized foreground style layer.
 */
- (instancetype)initWithIdentifier:(NSString *)identifier source:(MGLSource *)source NS_DESIGNATED_INITIALIZER;

#pragma mark Specifying a Style Layer’s Content

/**
 Identifier of the source from which the receiver obtains the data to style.
 */
@property (nonatomic, readonly, nullable) NSString *sourceIdentifier;

@end

NS_ASSUME_NONNULL_END
