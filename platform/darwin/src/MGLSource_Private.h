#import "MGLSource.h"

#include <mbgl/mbgl.hpp>
#include <mbgl/style/source.hpp>

@class MGLMapView;

@interface MGLSource (Private)

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

@end
