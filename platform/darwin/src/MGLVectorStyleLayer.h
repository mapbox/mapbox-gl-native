#import <Foundation/Foundation.h>

#import "MGLForegroundStyleLayer.h"

NS_ASSUME_NONNULL_BEGIN

/**
 `MGLVectorStyleLayer` is an abstract superclass for style layers whose content
 is defined by an `MGLGeoJSONSource` or `MGLVectorSource` object.
 
 Do not create instances of this class directly, and do not create your own
 subclasses of this class. Instead, create instances of the following concrete
 subclasses: `MGLCircleStyleLayer`, `MGLFillStyleLayer`, `MGLLineStyleLayer`,
 and `MGLSymbolStyleLayer`.
 */
@interface MGLVectorStyleLayer : MGLForegroundStyleLayer

#pragma mark Refining a Style Layer’s Content

/**
 Identifier of the layer within the source identified by the `sourceIdentifier`
 property from which the receiver obtains the data to style.
 */
@property (nonatomic, nullable) NSString *sourceLayerIdentifier;

/**
 A predicate that corresponds to the layer's <a href='https://www.mapbox.com/mapbox-gl-style-spec/#types-filter'>filter</a>.
 
 The predicate's left expression must be a string that identifies a feature
 property, or one of the special keys.
 */
@property (nonatomic, nullable) NSPredicate *predicate;

@end

NS_ASSUME_NONNULL_END
