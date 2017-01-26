#import <AppKit/AppKit.h>

#import "MGLFoundation.h"

NS_ASSUME_NONNULL_BEGIN

/**
 The `MGLAnnotationImage` class is responsible for presenting point-based
 annotations visually on an `MGLMapView` instance. Annotation image objects pair
 `NSImage` objects with annotation-related metadata. They may be recycled later
 and put into a reuse queue that is maintained by the map view.
 */
MGL_EXPORT
@interface MGLAnnotationImage : NSObject <NSSecureCoding>

#pragma mark Initializing and Preparing the Image Object

/**
 Initializes and returns a new annotation image object.

 @param image The image to display for the annotation.
 @param reuseIdentifier The string that identifies this annotation image in the
    reuse queue.
 @return The initialized annotation image object or `nil` if there was a problem
    initializing the object.
 */
+ (instancetype)annotationImageWithImage:(NSImage *)image reuseIdentifier:(NSString *)reuseIdentifier;

#pragma mark Getting and Setting Attributes

/** The image to display for the annotation. */
@property (nonatomic, readonly) NSImage *image;

/**
 The string that identifies this annotation image in the reuse queue.
 (read-only)

 You specify the reuse identifier when you create the image object. You use this
 type later to retrieve an annotation image object that was created previously
 but which is currently unused because its annotation is not on-screen.

 If you define distinctly different types of annotations (with distinctly
 different annotation images to go with them), you can differentiate between the
 annotation types by specifying different reuse identifiers for each one.
 */
@property (nonatomic, readonly) NSString *reuseIdentifier;

/**
 A Boolean value indicating whether the annotation is selectable.

 The default value of this property is `YES`. If the value of this property is
 `NO`, the annotation image ignores click events and cannot be selected.
 */
@property (nonatomic, getter=isSelectable) BOOL selectable;

/**
 The cursor that appears above any annotation using this annotation image.

 By default, this property is set to `nil`, representing the current cursor.
 */
@property (nonatomic, nullable) NSCursor *cursor;

@end

NS_ASSUME_NONNULL_END
