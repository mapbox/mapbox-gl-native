#import "MGLImageSource_Private.h"

#import "MGLGeometry_Private.h"
#import "MGLSource_Private.h"
#import "MGLTileSource_Private.h"
#import "NSURL+MGLAdditions.h"
#if TARGET_OS_IPHONE
    #import "UIImage+MGLAdditions.h"
#else
    #import "NSImage+MGLAdditions.h"
#endif

#include <mbgl/style/sources/image_source.hpp>
#include <mbgl/util/premultiply.hpp>

@interface MGLImageSource ()

@property (nonatomic, readonly) mbgl::style::ImageSource *rawSource;

@end

@implementation MGLImageSource

- (instancetype)initWithIdentifier:(NSString *)identifier coordinates:(MGLCoordinateQuad)coordinateQuad {

    const auto coordsArray = MGLLatLngArrayFromCoordinateQuad(coordinateQuad);
    auto source = std::make_unique<mbgl::style::ImageSource>(identifier.UTF8String, coordsArray);
    return self = [super initWithPendingSource:std::move(source)];
}


- (instancetype)initWithIdentifier:(NSString *)identifier coordinates:(MGLCoordinateQuad)coordinateQuad imageURL:(NSURL *)url {
    self =  [self initWithIdentifier:identifier coordinates: coordinateQuad];
    self.URL = url;
    return self;
}


- (instancetype)initWithIdentifier:(NSString *)identifier coordinates:(MGLCoordinateQuad)coordinateQuad image:(MGLImage *)image {
    self =  [self initWithIdentifier:identifier coordinates: coordinateQuad];
    self.image = image;

    return self;
}

- (instancetype)initWithIdentifier:(NSString *)identifier bounds:(MGLCoordinateBounds)bounds {
    self = [self initWithIdentifier:identifier coordinates:MGLCoordinateQuadFromCoordinateBounds(bounds)];
    return self;
}

- (instancetype)initWithIdentifier:(NSString *)identifier bounds:(MGLCoordinateBounds)bounds imageURL:(NSURL *)url {
    self = [self initWithIdentifier:identifier coordinates:MGLCoordinateQuadFromCoordinateBounds(bounds) imageURL:url];
    return self;
}

- (instancetype)initWithIdentifier:(NSString *)identifier bounds:(MGLCoordinateBounds)bounds image:(MGLImage *)image {
    self = [self initWithIdentifier:identifier coordinates:MGLCoordinateQuadFromCoordinateBounds(bounds) image:image];
    return self;
}

- (NSURL *)URL {
    auto url = self.rawSource->getURL();
    return url ? [NSURL URLWithString:@(url->c_str())] : nil;
}

- (void)setURL:(NSURL *)url {
    if (url) {
        self.rawSource->setURL(url.mgl_URLByStandardizingScheme.absoluteString.UTF8String);
        _image = nil;
    } else {
        self.image = nullptr;
    }
}

- (void)setImage:(MGLImage *)image {
    if (image != nullptr) {
        mbgl::UnassociatedImage unassociatedImage = mbgl::util::unpremultiply(image.mgl_PremultipliedImage);
        self.rawSource->setImage(std::move(unassociatedImage));
    }
    _image = image;
}

- (MGLCoordinateQuad)coordinates {
    return MGLCoordinateQuadFromLatLngArray(self.rawSource->getCoordinates());
}

- (void)setCoordinates: (MGLCoordinateQuad)coordinateQuad {
    self.rawSource->setCoordinates(MGLLatLngArrayFromCoordinateQuad(coordinateQuad));
}

- (NSString *)description {
    return [NSString stringWithFormat:@"<%@: %p; identifier = %@; URL = %@; image = %@>",
            NSStringFromClass([self class]), (void *)self, self.identifier, self.URL, self.image];
}

- (mbgl::style::ImageSource *)rawSource {
    return (mbgl::style::ImageSource *)super.rawSource;
}

- (NSString *)attributionHTMLString {
    auto attribution = self.rawSource->getAttribution();
    return attribution ? @(attribution->c_str()) : nil;
}

@end
