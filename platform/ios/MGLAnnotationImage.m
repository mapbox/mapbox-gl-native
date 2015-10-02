#import "MGLAnnotationImage.h"

@interface MGLAnnotationImage ()

@property (nonatomic) UIImage *image;
@property (nonatomic) NSString *reuseIdentifier;

@end

@implementation MGLAnnotationImage

+ (instancetype)annotationImageWithImage:(UIImage *)image reuseIdentifier:(NSString *)reuseIdentifier
{
    return [[self alloc] initWithImage:image reuseIdentifier:reuseIdentifier];
}

- (instancetype)initWithImage:(UIImage *)image reuseIdentifier:(NSString *)reuseIdentifier
{
    self = [super init];

    if (self)
    {
        _image = image;
        _reuseIdentifier = [reuseIdentifier copy];
        _enabled = YES;
    }

    return self;
}

@end
