#import <Foundation/Foundation.h>
#import <CoreGraphics/CoreGraphics.h>

#import "MGLFoundation.h"

#pragma once

#if TARGET_OS_IPHONE
@class UIImage;
#define MGLImage UIImage
#else
@class NSImage;
#define MGLImage NSImage
#endif

#if TARGET_OS_IPHONE
@class UIColor;
#define MGLColor UIColor
#else
@class NSColor;
#define MGLColor NSColor
#endif

NS_ASSUME_NONNULL_BEGIN

#ifndef NS_STRING_ENUM
    #define NS_STRING_ENUM
    #define NS_EXTENSIBLE_STRING_ENUM
    typedef NSString *NSErrorDomain;
    typedef NSString *NSNotificationName;
#endif

/** Indicates an error occurred in the Mapbox SDK. */
extern MGL_EXPORT NSErrorDomain const MGLErrorDomain;

/** Error constants for the Mapbox SDK. */
typedef NS_ENUM(NSInteger, MGLErrorCode) {
    /** An unknown error occurred. */
    MGLErrorCodeUnknown = -1,
    /** The resource could not be found. */
    MGLErrorCodeNotFound = 1,
    /** The connection received an invalid server response. */
    MGLErrorCodeBadServerResponse = 2,
    /** An attempt to establish a connection failed. */
    MGLErrorCodeConnectionFailed = 3,
    /** A style parse error occurred while attempting to load the map. */
    MGLErrorCodeParseStyleFailed = 4,
    /** An attempt to load the style failed. */
    MGLErrorCodeLoadStyleFailed = 5,
    /** An error occurred while snapshotting the map. */
    MGLErrorCodeSnapshotFailed = 6,
};

/** Options for enabling debugging features in an `MGLMapView` instance. */
typedef NS_OPTIONS(NSUInteger, MGLMapDebugMaskOptions) {
    /** Edges of tile boundaries are shown as thick, red lines to help diagnose
        tile clipping issues. */
    MGLMapDebugTileBoundariesMask = 1 << 1,
    /** Each tile shows its tile coordinate (x/y/z) in the upper-left corner. */
    MGLMapDebugTileInfoMask = 1 << 2,
    /** Each tile shows a timestamp indicating when it was loaded. */
    MGLMapDebugTimestampsMask = 1 << 3,
    /** Edges of glyphs and symbols are shown as faint, green lines to help
        diagnose collision and label placement issues. */
    MGLMapDebugCollisionBoxesMask = 1 << 4,
    /** Each drawing operation is replaced by a translucent fill. Overlapping
        drawing operations appear more prominent to help diagnose overdrawing.
        @note This option does nothing in Release builds of the SDK. */
    MGLMapDebugOverdrawVisualizationMask = 1 << 5,
#if !TARGET_OS_IPHONE
    /** The stencil buffer is shown instead of the color buffer.
        @note This option does nothing in Release builds of the SDK. */
    MGLMapDebugStencilBufferMask = 1 << 6,
    /** The depth buffer is shown instead of the color buffer.
        @note This option does nothing in Release builds of the SDK. */
    MGLMapDebugDepthBufferMask = 1 << 7,
#endif
};

/**
 A structure containing information about a transition.
 */
typedef struct __attribute__((objc_boxable)) MGLTransition {
    /**
     The amount of time the animation should take, not including the delay.
     */
    NSTimeInterval duration;
    
    /**
     The amount of time in seconds to wait before beginning the animation.
     */
    NSTimeInterval delay;
} MGLTransition;

/**
 Creates a new `MGLTransition` from the given duration and delay.
 
 @param duration The amount of time the animation should take, not including 
 the delay.
 @param delay The amount of time in seconds to wait before beginning the 
 animation.
 
 @return Returns a `MGLTransition` struct containing the transition attributes.
 */
NS_INLINE MGLTransition MGLTransitionMake(NSTimeInterval duration, NSTimeInterval delay) {
    MGLTransition transition;
    transition.duration = duration;
    transition.delay = delay;
    
    return transition;
}

NS_ASSUME_NONNULL_END

#ifndef NS_ARRAY_OF
    // Foundation collection classes adopted lightweight generics in iOS 9.0 and OS X 10.11 SDKs.
    #if __has_feature(objc_generics) && (__IPHONE_OS_VERSION_MAX_ALLOWED >= 90000 || __MAC_OS_X_VERSION_MAX_ALLOWED >= 101100)
        /** Inserts a type specifier for a pointer to a lightweight generic with the given collection and object classes. Use a `*` for any non-`id` object classes but no `*` for the collection class. */
        #define NS_ARRAY_OF(ObjectClass...)                 NSArray <ObjectClass>
        #define NS_MUTABLE_ARRAY_OF(ObjectClass...)         NSMutableArray <ObjectClass>
        #define NS_SET_OF(ObjectClass...)                   NSSet <ObjectClass>
        #define NS_MUTABLE_SET_OF(ObjectClass...)           NSMutableSet <ObjectClass>
        #define NS_DICTIONARY_OF(ObjectClass...)            NSDictionary <ObjectClass>
        #define NS_MUTABLE_DICTIONARY_OF(ObjectClass...)    NSMutableDictionary <ObjectClass>
    #else
        #define NS_ARRAY_OF(ObjectClass...)                 NSArray
        #define NS_MUTABLE_ARRAY_OF(ObjectClass...)         NSMutableArray
        #define NS_SET_OF(ObjectClass...)                   NSSet
        #define NS_MUTABLE_SET_OF(ObjectClass...)           NSMutableSet
        #define NS_DICTIONARY_OF(ObjectClass...)            NSDictionary
        #define NS_MUTABLE_DICTIONARY_OF(ObjectClass...)    NSMutableDictionary
    #endif
#endif
