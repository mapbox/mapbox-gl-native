#import <Foundation/Foundation.h>
#import <CoreLocation/CoreLocation.h>

#import "MGLAnnotation.h"

#import "MGLTypes.h"

NS_ASSUME_NONNULL_BEGIN

/** The MGLUserLocation class defines a specific type of annotation that identifies the user’s current location. You do not create instances of this class directly. Instead, you retrieve an existing MGLUserLocation object from the userLocation property of the map view displayed in your application. */
@interface MGLUserLocation : NSObject <MGLAnnotation>

#pragma mark Determining the User’s Position

/**
 The current location of the device. (read-only)

 This property contains `nil` if the map view is not currently showing the user location or if the user’s location has not yet been determined.
 */
@property (nonatomic, readonly, nullable) CLLocation *location;

/** A Boolean value indicating whether the user’s location is currently being updated. (read-only) */
@property (nonatomic, readonly, getter=isUpdating) BOOL updating;

/**
 The heading of the user location. (read-only)
 
 This property is `nil` if the user location tracking mode is not `MGLUserTrackingModeFollowWithHeading`.
 */
@property (nonatomic, readonly, nullable) CLHeading *heading;

#pragma mark Accessing the User Annotation Text

/** The title to display for the user location annotation. */
@property (nonatomic, copy) NSString *title;

/** The subtitle to display for the user location annotation. */
@property (nonatomic, copy, nullable) NSString *subtitle;

@end

NS_ASSUME_NONNULL_END
