#import "MGLSource.h"

#include <mbgl/mbgl.hpp>
#include <mbgl/style/source.hpp>

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
 A std::unique_ptr<>, which is present only for objects that were created 
 independently (not obtained by `mbgl::Map getSource`) and have not yet
 been added via `mbgl::Map addSource`. Once a source is added, ownership of the 
 object is transferred to the `mbgl::Map` and `pendingSource` is NULL. If the
 `MGLSource` object's mbgl source is in that state, the mbgl source can still be
 changed but the changes will not be visible until the `MGLSource` is added back
 via `-[MGLStyle addSource:]` and styled with an `MGLLayer`.
 */
@property (nonatomic) std::unique_ptr<mbgl::style::Source> pendingSource;

@end
