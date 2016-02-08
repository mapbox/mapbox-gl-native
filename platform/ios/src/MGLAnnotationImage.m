#import "MGLAnnotationImage_Private.h"

@interface MGLAnnotationImage ()

@property (nonatomic, strong) NSString *reuseIdentifier;
@property (nonatomic, weak) id<MGLAnnotationImageDelegate> delegate;

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

- (void)setImage:(UIImage *)image {
    _image = image;
    [self.delegate annotationImageNeedsRedisplay:self];
}

@end
