#import "MGLAnnotationImage_Private.h"

@interface MGLAnnotationImage ()

@property (nonatomic, strong) NSString *reuseIdentifier;
@property (nonatomic, strong, nullable) NSString *styleIconIdentifier;

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

+ (BOOL)supportsSecureCoding {
    return YES;
}

- (instancetype)initWithCoder:(NSCoder *)decoder {
    if (self = [super init]) {
        _image = [decoder decodeObjectOfClass:[UIImage class] forKey:@"image"];
        _reuseIdentifier = [decoder decodeObjectOfClass:[NSString class] forKey:@"reuseIdentifier"];
        _enabled = [decoder decodeBoolForKey:@"enabled"];
    }
    return self;
}

- (void)encodeWithCoder:(NSCoder *)coder {
    [coder encodeObject:_image forKey:@"image"];
    [coder encodeObject:_reuseIdentifier forKey:@"reuseIdentifier"];
    [coder encodeBool:_enabled forKey:@"enabled"];
}

- (BOOL)isEqual:(id)other {
    if (self == other) return YES;
    if (![other isKindOfClass:[MGLAnnotationImage class]]) return NO;

    MGLAnnotationImage *otherAnnotationImage = other;

    return ((!_reuseIdentifier && !otherAnnotationImage.reuseIdentifier)
            || [_reuseIdentifier isEqualToString:otherAnnotationImage.reuseIdentifier])
    && _enabled == otherAnnotationImage.enabled
    && (_image == otherAnnotationImage.image || [UIImagePNGRepresentation(_image) isEqualToData:UIImagePNGRepresentation(otherAnnotationImage.image)]);
}

- (NSUInteger)hash {
    return _reuseIdentifier.hash + _enabled + _image.hash;
}

- (void)setImage:(UIImage *)image {
    _image = image;
    [self.delegate annotationImageNeedsRedisplay:self];
}

@end
