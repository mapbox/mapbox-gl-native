#import <Foundation/Foundation.h>

#import "MGLAnnotation.h"

#import "MGLTypes.h"

NS_ASSUME_NONNULL_BEGIN

/** The `MGLShape` class is an abstract class that defines the basic properties for all shape-based annotation objects. This class must be subclassed and cannot be used as is. Subclasses are responsible for defining the geometry of the shape and providing an appropriate value for the coordinate property inherited from the `MGLAnnotation` protocol. */
@interface MGLShape : NSObject <MGLAnnotation>

/** The title of the shape annotation. The default value of this property is `nil`. */
@property (nonatomic, copy, nullable) NSString *title;

/** The subtitle of the shape annotation. The default value of this property is `nil`. */
@property (nonatomic, copy, nullable) NSString *subtitle;

#if !TARGET_OS_IPHONE

/** The tooltip of the shape annotation. The default value of this property is `nil`. */
@property (nonatomic, copy, nullable) NSString *toolTip;

#endif

@end

NS_ASSUME_NONNULL_END
