#import <Foundation/Foundation.h>

// style property value types
//
extern NSString *const MGLStyleValueTypeBoolean;
extern NSString *const MGLStyleValueTypeNumber;
extern NSString *const MGLStyleValueTypeNumberPair;
extern NSString *const MGLStyleValueTypeColor;
extern NSString *const MGLStyleValueTypeString;

// style property function types
//
extern NSString *const MGLStyleValueTypeFunctionMinimumZoom;
extern NSString *const MGLStyleValueTypeFunctionMaximumZoom;
extern NSString *const MGLStyleValueTypeFunctionLinear;
extern NSString *const MGLStyleValueTypeFunctionExponential;
extern NSString *const MGLStyleValueTypeFunctionStops;

/// The degree to which the map view tracks the user’s location.
typedef NS_ENUM(NSUInteger, MGLUserTrackingMode)
{
    MGLUserTrackingModeNone              = 0,   ///< does not track the user’s location or heading
    MGLUserTrackingModeFollow            = 1,   ///< tracks the user’s location
    MGLUserTrackingModeFollowWithHeading = 2,   ///< tracks the user’s location and heading
};
