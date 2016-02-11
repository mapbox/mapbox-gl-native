#import <MGLAnnotationImage.h>

@interface MGLAnnotationImage ()

@property (nonatomic) NSImage *image;
@property (nonatomic) NSString *reuseIdentifier;

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

@end
