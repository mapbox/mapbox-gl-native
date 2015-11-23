#import <AppKit/AppKit.h>

#import "MGLTypes.h"

NS_ASSUME_NONNULL_BEGIN

@interface MGLAnnotationImage : NSObject

+ (instancetype)annotationImageWithImage:(NSImage *)image reuseIdentifier:(NSString *)reuseIdentifier;

@property (nonatomic, readonly) NSImage *image;
@property (nonatomic, readonly) NSString *reuseIdentifier;
@property (nonatomic, getter=isSelectable) BOOL selectable;

@end

NS_ASSUME_NONNULL_END
