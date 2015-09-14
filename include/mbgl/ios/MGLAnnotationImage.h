#import <UIKit/UIKit.h>

#import "MGLTypes.h"

NS_ASSUME_NONNULL_BEGIN

@interface MGLAnnotationImage : NSObject

@property (nonatomic, readonly) UIImage *image;
@property (nonatomic, readonly) NSString *reuseIdentifier;

+ (instancetype)annotationImageWithImage:(UIImage *)image reuseIdentifier:(NSString *)reuseIdentifier;

@end

NS_ASSUME_NONNULL_END
