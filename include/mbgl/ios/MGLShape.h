#import <Foundation/Foundation.h>

#import "MGLAnnotation.h"

/** The `MGLShape` class is an abstract class that defines the basic properties for all shape-based annotation objects. This class must be subclassed and cannot be used as is. Subclasses are responsible for defining the geometry of the shape and providing an appropriate value for the coordinate property inherited from the `MGLAnnotation` protocol. */
@interface MGLShape : NSObject <MGLAnnotation>

/** The title of the shape annotation. The default value of this property is `nil`. */
@property (nonatomic, copy) NSString *title;

/** The subtitle of the shape annotation. The default value of this property is `nil`. */
@property (nonatomic, copy) NSString *subtitle;

@end
