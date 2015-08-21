#import <Foundation/Foundation.h>

#pragma once

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
    MGLUserTrackingModeFollowWithHeading,
    /** The map follows the user location and rotates when the course changes. */
    MGLUserTrackingModeFollowWithCourse,
};

NS_ASSUME_NONNULL_END

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wvariadic-macros"
    #if __has_feature(objc_generics)
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
#pragma clang diagnostic pop
