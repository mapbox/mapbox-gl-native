#import <XCTest/XCTest.h>

#import <Mapbox/Mapbox.h>

@interface MGLImageSourceTests : XCTestCase

@end

@implementation MGLImageSourceTests


- (void)testMGLImageSourceWithImageURL {

    MGLCoordinateQuad quad = { { 80, 37}, { 81, 37}, { 81, 39}, { 80, 39}};
    MGLImageSource *source = [[MGLImageSource alloc] initWithIdentifier:@"source-id" coordinateQuad:quad URL:[NSURL URLWithString:@"http://host/image.png"]];
    
    XCTAssertNotNil(source.URL);
    XCTAssertEqualObjects(source.URL.absoluteString, @"http://host/image.png");
    XCTAssertNil(source.image);
}

- (void)testMGLImageSourceWithImage {

    NSString *imageName = @"RadarImage";
#if TARGET_OS_IPHONE
    MGLImage *image = [MGLImage imageNamed:imageName
                                  inBundle:[NSBundle bundleForClass:[self class]]
             compatibleWithTraitCollection:nil];
#else
    MGLImage *image = [[NSBundle bundleForClass:[self class]] imageForResource:imageName];
#endif
    XCTAssertNotNil(image);

    MGLCoordinateQuad quad = { { 80, 37}, { 81, 37}, { 81, 39}, { 80, 39}};
    MGLImageSource *source = [[MGLImageSource alloc] initWithIdentifier:@"source-id" coordinateQuad:quad image:image];
    
    XCTAssertNotNil(source.image);
    XCTAssertEqualObjects(source.image, image);
    XCTAssertNil(source.URL);
}

@end
