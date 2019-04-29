#import "MGLCircle.h"

#import <mbgl/annotation/annotation.hpp>

NS_ASSUME_NONNULL_BEGIN

@protocol MGLMultiPointDelegate;

@interface MGLCircle (Private)

/**
 The optimal number of vertices in the circle’s polygonal approximation.
 */
@property (nonatomic, readonly) NSUInteger numberOfVertices;

/**
 Returns a linear ring with the given number of vertices.
 */
- (mbgl::LinearRing<double>)linearRingWithNumberOfVertices:(NSUInteger)numberOfVertices;

/** Constructs a circle annotation object, asking the delegate for style values. */
- (mbgl::Annotation)annotationObjectWithDelegate:(id <MGLMultiPointDelegate>)delegate;

@end

NS_ASSUME_NONNULL_END
