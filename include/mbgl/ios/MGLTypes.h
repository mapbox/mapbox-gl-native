#import <Foundation/Foundation.h>
#import <CoreLocation/CoreLocation.h>

#if !__has_feature(nullability)
    #define NS_ASSUME_NONNULL_BEGIN
    #define NS_ASSUME_NONNULL_END
    #define nullable
    #define nonnull
    #define null_resettable
#endif

NS_ASSUME_NONNULL_BEGIN

extern NSString * const MGLErrorDomain;

/** The mode used to track the user location on the map. */
typedef NS_ENUM(NSUInteger, MGLUserTrackingMode) {
    /** The map does not follow the user location. */
    MGLUserTrackingModeNone              = 0,
    /** The map follows the user location. */
    MGLUserTrackingModeFollow,
    /** The map follows the user location and rotates when the heading changes. */
    MGLUserTrackingModeFollowWithHeading
};

typedef struct {
    CLLocationCoordinate2D sw;
    CLLocationCoordinate2D ne;
} MGLCoordinateBounds;

NS_ASSUME_NONNULL_END
