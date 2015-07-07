#import <UIKit/UIKit.h>

@interface MGLAnnotationImage : NSObject

@property (nonatomic, readonly) UIImage *image;
@property (nonatomic, readonly) NSString *reuseIdentifier;

+ (instancetype)annotationImageWithImage:(UIImage *)image reuseIdentifier:(NSString *)reuseIdentifier;

@end
