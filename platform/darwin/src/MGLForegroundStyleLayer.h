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

- (instancetype)init __attribute__((unavailable("Use -init methods of concrete subclasses instead.")));

#pragma mark Specifying a Style Layer’s Content

/**
 Identifier of the source from which the receiver obtains the data to style.
 */
@property (nonatomic, readonly, nullable) NSString *sourceIdentifier;

@end

NS_ASSUME_NONNULL_END
