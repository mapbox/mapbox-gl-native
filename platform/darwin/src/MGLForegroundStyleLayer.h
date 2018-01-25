#import <Foundation/Foundation.h>

#import "MGLFoundation.h"
#import "MGLStyleLayer.h"

NS_ASSUME_NONNULL_BEGIN

@class MGLSource;

/**
 `MGLForegroundStyleLayer` is an abstract superclass for style layers whose
 content is defined by an `MGLSource` object.

 Create instances of `MGLRasterStyleLayer`, `MGLHillshadeStyleLayer`, and the
 concrete subclasses of `MGLVectorStyleLayer` in order to use
 `MGLForegroundStyleLayer`'s methods. Do not create instances of
 `MGLForegroundStyleLayer` directly, and do not create your own subclasses of
 this class.
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
