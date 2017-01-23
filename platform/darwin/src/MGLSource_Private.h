#import "MGLSource.h"

NS_ASSUME_NONNULL_BEGIN

namespace mbgl {
    namespace style {
        class Source;
    }
}

@class MGLMapView;

@interface MGLSource (Private)

/**
 Initializes and returns a source with a raw pointer to the backing store.
 */
- (instancetype)initWithRawSource:(mbgl::style::Source *)rawSource;

/**
 A raw pointer to the mbgl object, which is always initialized, either to the 
 value returned by `mbgl::Map getSource`, or for independently created objects,
 to the pointer value held in `pendingSource`. In the latter case, this raw
 pointer value stays even after ownership of the object is transferred via 
 `mbgl::Map addSource`.
 */
@property (nonatomic) mbgl::style::Source *rawSource;

/**
 Adds the mbgl source that this object represents to the mbgl map.
 
 Once a mbgl source is added, ownership of the object is transferred to the
 `mbgl::Map` and this object no longer has an active unique_ptr reference to the
 `mbgl::Source`. If this object's mbgl source is in that state, the mbgl source 
 can still be changed but the changes will not be visible until the `MGLSource` 
 is added back to the map via `-[MGLStyle addSource:]` and styled with a 
 `MGLLayer`.
 */
- (void)addToMapView:(MGLMapView *)mapView;

/**
 Removes the mbgl source that this object represents from the mbgl map.
 
 When a mbgl source is removed, ownership of the object is transferred back
 to the `MGLSource` instance and the unique_ptr reference is valid again. It is
 safe to add the source back to the style after it is removed.
 */
- (void)removeFromMapView:(MGLMapView *)mapView;

@end

NS_ASSUME_NONNULL_END
