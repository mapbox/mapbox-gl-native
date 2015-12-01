#import <AppKit/AppKit.h>

#import "MGLTypes.h"

NS_ASSUME_NONNULL_BEGIN

@interface MGLAnnotationImage : NSObject

+ (instancetype)annotationImageWithImage:(NSImage *)image reuseIdentifier:(NSString *)reuseIdentifier;

@property (nonatomic, readonly) NSImage *image;
@property (nonatomic, readonly) NSString *reuseIdentifier;
@property (nonatomic, getter=isSelectable) BOOL selectable;

/** The cursor that appears above any annotation using this annotation image. By default this property is set to `nil`, representing the current cursor. */
@property (nonatomic, nullable) NSCursor *cursor;

@end

NS_ASSUME_NONNULL_END
