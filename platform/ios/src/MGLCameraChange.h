#import "MGLFoundation.h"

typedef NS_OPTIONS(NSUInteger, MGLCameraChangeReason)
{
    /// No reason for the camera move is specified.
    MGLCameraChangeReasonNone = 0,

    /// Set when a public API that moves the camera is called. This may be set for some gestures
    MGLCameraChangeReasonProgrammatic = 1 << 0,

    /// User tapped the compass to reset to North
    MGLCameraChangeReasonResetNorth = 1 << 1,// Tap on compass

    /// User panned the map.
    MGLCameraChangeReasonGesturePan = 1 << 2,

    /// User pinched to zoom in/out
    MGLCameraChangeReasonGesturePinch = 1 << 3,

    // User rotated the map
    MGLCameraChangeReasonGestureRotate = 1 << 4,

    /// User zoomed the map in
    MGLCameraChangeReasonGestureZoomIn = 1 << 5,// One finger double tap

    /// User zoomed the map out
    MGLCameraChangeReasonGestureZoomOut = 1 << 6,// Two finger single tap

    /// User long pressed on the map for a quick zoom
    MGLCameraChangeReasonGestureQuickZoom = 1 << 7,// Long press

    // User panned with two fingers to tilt the map
    MGLCameraChangeReasonGesturePitch = 1 << 8// Two finger drag
};

