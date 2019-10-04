#import "MGLImageSource.h"

#import "MGLGeometry_Private.h"
#import "MGLLoggingConfiguration_Private.h"
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
- (instancetype)initWithIdentifier:(NSString *)identifier coordinateQuad:(MGLCoordinateQuad)coordinateQuad NS_DESIGNATED_INITIALIZER;

@property (nonatomic, readonly) mbgl::style::ImageSource *rawSource;

@end

@implementation MGLImageSource

- (instancetype)initWithIdentifier:(NSString *)identifier coordinateQuad:(MGLCoordinateQuad)coordinateQuad {

    const auto coordsArray = MGLLatLngArrayFromCoordinateQuad(coordinateQuad);
    auto source = std::make_unique<mbgl::style::ImageSource>(identifier.UTF8String, coordsArray);
    return self = [super initWithPendingSource:std::move(source)];
}


- (instancetype)initWithIdentifier:(NSString *)identifier coordinateQuad:(MGLCoordinateQuad)coordinateQuad URL:(NSURL *)url {
    self =  [self initWithIdentifier:identifier coordinateQuad: coordinateQuad];
    self.URL = url;
    return self;
}


- (instancetype)initWithIdentifier:(NSString *)identifier coordinateQuad:(MGLCoordinateQuad)coordinateQuad image:(MGLImage *)image {
    self =  [self initWithIdentifier:identifier coordinateQuad: coordinateQuad];
    self.image = image;

    return self;
}

- (NSURL *)URL {
    MGLAssertStyleSourceIsValid();
    auto url = self.rawSource->getURL();
    return url ? [NSURL URLWithString:@(url->c_str())] : nil;
}

- (void)setURL:(NSURL *)url {
    MGLAssertStyleSourceIsValid();
    if (url) {
        self.rawSource->setURL(url.mgl_URLByStandardizingScheme.absoluteString.UTF8String);
        _image = nil;
    } else {
        self.image = nullptr;
    }
}

- (void)setImage:(MGLImage *)image {
    MGLAssertStyleSourceIsValid();
    if (image != nullptr) {
        self.rawSource->setImage(image.mgl_premultipliedImage);
    } else {
        self.rawSource->setImage(mbgl::PremultipliedImage({0,0}));
    }
    _image = image;
}

- (MGLCoordinateQuad)coordinates {
    MGLAssertStyleSourceIsValid();
    return MGLCoordinateQuadFromLatLngArray(self.rawSource->getCoordinates());
}

- (void)setCoordinates: (MGLCoordinateQuad)coordinateQuad {
    MGLAssertStyleSourceIsValid();
    self.rawSource->setCoordinates(MGLLatLngArrayFromCoordinateQuad(coordinateQuad));
}

- (NSString *)description {
    if (self.rawSource) {
        return [NSString stringWithFormat:@"<%@: %p; identifier = %@; coordinates = %@; URL = %@; image = %@>",
                NSStringFromClass([self class]), (void *)self, self.identifier,
                MGLStringFromCoordinateQuad(self.coordinates),
                self.URL,
                self.image];
    }
    else {
        return [NSString stringWithFormat:@"<%@: %p; identifier = %@; coordinates = <unknown>; URL = <unknown>; image = %@>",
                NSStringFromClass([self class]), (void *)self, self.identifier, self.image];
    }
}

- (mbgl::style::ImageSource *)rawSource {
    return (mbgl::style::ImageSource *)super.rawSource;
}

- (NSString *)attributionHTMLString {
    if (!self.rawSource) {
        MGLAssert(0, @"Source with identifier `%@` was invalidated after a style change", self.identifier);
        return nil;
    }

    auto attribution = self.rawSource->getAttribution();
    return attribution ? @(attribution->c_str()) : nil;
}

@end
