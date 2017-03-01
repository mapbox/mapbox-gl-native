#import "MGLAnnotationImage_Private.h"

@interface MGLAnnotationImage ()

@property (nonatomic) NSImage *image;
@property (nonatomic) NSString *reuseIdentifier;
@property (nonatomic, strong, nullable) NSString *styleIconIdentifier;

@end

@implementation MGLAnnotationImage

+ (instancetype)annotationImageWithImage:(NSImage *)image reuseIdentifier:(NSString *)reuseIdentifier {
    return [[self alloc] initWithImage:image reuseIdentifier:reuseIdentifier];
}

- (instancetype)initWithImage:(NSImage *)image reuseIdentifier:(NSString *)reuseIdentifier {
    if (self = [super init]) {
        _image = image;
        _reuseIdentifier = [reuseIdentifier copy];
        _selectable = YES;
    }
    return self;
}

+ (BOOL)supportsSecureCoding {
    return YES;
}

- (instancetype)initWithCoder:(NSCoder *)decoder {
    if (self = [super init]) {
        _image = [decoder decodeObjectOfClass:[NSImage class] forKey:@"image"];
        _reuseIdentifier = [decoder decodeObjectOfClass:[NSString class] forKey:@"reuseIdentifier"];
        _cursor = [decoder decodeObjectOfClass:[NSCursor class] forKey:@"cursor"];
        _selectable = [decoder decodeBoolForKey:@"selectable"];
    }
    return self;
}

- (void)encodeWithCoder:(NSCoder *)coder {
    [coder encodeObject:_image forKey:@"image"];
    [coder encodeObject:_reuseIdentifier forKey:@"reuseIdentifier"];
    [coder encodeObject:_cursor forKey:@"cursor"];
    [coder encodeBool:_selectable forKey:@"selectable"];
}

- (BOOL)isEqual:(id)other {
    if (self == other) return YES;
    if (![other isKindOfClass:[MGLAnnotationImage class]]) return NO;

    MGLAnnotationImage *otherAnnotationImage = other;

    return ((!_reuseIdentifier && !otherAnnotationImage.reuseIdentifier) || [_reuseIdentifier isEqualToString:otherAnnotationImage.reuseIdentifier])
    && _selectable == otherAnnotationImage.selectable
    && ((!_cursor && !otherAnnotationImage.cursor) || [_cursor isEqual:otherAnnotationImage.cursor])
    && (_image == otherAnnotationImage.image || [[_image TIFFRepresentation] isEqualToData:[otherAnnotationImage.image TIFFRepresentation]]);
}

- (NSUInteger)hash {
    return _reuseIdentifier.hash + @(_selectable).hash + _image.hash;
}

@end
