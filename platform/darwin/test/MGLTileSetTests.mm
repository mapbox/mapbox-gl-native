#import <XCTest/XCTest.h>

#import <Mapbox/Mapbox.h>
#import "MGLTileSource_Private.h"
#import "MGLGeometry_Private.h"

#include <mbgl/util/tileset.hpp>

@interface MGLTileSetTests : XCTestCase

@end

@implementation MGLTileSetTests

- (void)testTileSetFromTileURLTemplates {
    // a tile set that provides an mbgl tile set
    NSArray *tileURLTemplates = @[@"tile.1", @"tile.2", @"tile.3"];
    mbgl::Tileset tileSet = MGLTileSetFromTileURLTemplates(tileURLTemplates, nil);

    // has the correct URL templates
    XCTAssertEqual(tileSet.tiles.size(), 3);
    XCTAssertEqual(tileSet.tiles[0], "tile.1");
    XCTAssertEqual(tileSet.tiles[1], "tile.2");
    XCTAssertEqual(tileSet.tiles[2], "tile.3");

    // has the default scheme
    XCTAssertEqual(tileSet.scheme, mbgl::Tileset::Scheme::XYZ);

    // when the tile set has no min or max zoom level set
    // the mbgl object has default values for min and max zoom level
    XCTAssertEqual(tileSet.zoomRange.min, 0);
    XCTAssertEqual(tileSet.zoomRange.max, 22);

    // when the tile set has min and/or max zoom level set
    tileSet = MGLTileSetFromTileURLTemplates(@[@"tile.1"], @{
        MGLTileSourceOptionMinimumZoomLevel: @1,
        MGLTileSourceOptionMaximumZoomLevel: @2,
    });

    // the mbgl object reflects the set values for min and max zoom level
    XCTAssertEqual(tileSet.zoomRange.min, 1);
    XCTAssertEqual(tileSet.zoomRange.max, 2);

    // when the tile set has a bounds set
    MGLCoordinateBounds bounds = MGLCoordinateBoundsMake(CLLocationCoordinate2DMake(12, 34), CLLocationCoordinate2DMake(56, 78));
    tileSet = MGLTileSetFromTileURLTemplates(@[@"tile.1"], @{
        MGLTileSourceOptionCoordinateBounds: @(bounds),
    });

    // the mbgl object reflects the set values for the bounds
    XCTAssert(!!tileSet.bounds, @"The bounds are set after setting the bounds");
    if (tileSet.bounds) {
        MGLCoordinateBounds actual = MGLCoordinateBoundsFromLatLngBounds(*tileSet.bounds);
        XCTAssert(MGLCoordinateBoundsEqualToCoordinateBounds(bounds, actual), @"The bounds round-trip");
    }

    // when the tile set has an attribution
    NSString *attribution = @"my tileset © ©️🎈";
    tileSet = MGLTileSetFromTileURLTemplates(tileURLTemplates, @{
        MGLTileSourceOptionAttributionHTMLString: attribution,
    });

    // the attribution is reflected by the mbgl tileset
    XCTAssertEqual(tileSet.attribution, attribution.UTF8String);

    // when the tile set has attribution infos
    MGLAttributionInfo *mapboxInfo = [[MGLAttributionInfo alloc] initWithTitle:[[NSAttributedString alloc] initWithString:@"Mapbox"]
                                                                           URL:[NSURL URLWithString:@"https://www.mapbox.com/"]];
    NSAttributedString *gl = [[NSAttributedString alloc] initWithString:@"GL" attributes:@{
        NSBackgroundColorAttributeName: [MGLColor redColor],
    }];
    MGLAttributionInfo *glInfo = [[MGLAttributionInfo alloc] initWithTitle:gl URL:nil];
    tileSet = MGLTileSetFromTileURLTemplates(tileURLTemplates, @{
        MGLTileSourceOptionAttributionInfos: @[mapboxInfo, glInfo],
    });

    // the attribution is reflected by the mbgl tileset
#if TARGET_OS_IPHONE
    NSString *html = (@"<font style=\"font-family: 'Helvetica'; font-weight: normal; font-style: normal; font-size: 12.00pt\">"
                      @"<a href=\"https://www.mapbox.com/\">Mapbox</a> </font>"
                      @"<font style=\"font-family: 'Helvetica'; font-weight: normal; font-style: normal; font-size: 12.00pt; background-color: #ff0000\">GL</font>\n");
#else
    NSString *html = (@"<font face=\"Helvetica\" size=\"3\" style=\"font: 12.0px Helvetica\">"
                      @"<a href=\"https://www.mapbox.com/\">Mapbox</a> </font>"
                      @"<font face=\"Helvetica\" size=\"3\" style=\"font: 12.0px Helvetica; background-color: #ff2600\">GL</font>\n");
#endif
    XCTAssertEqualObjects(@(tileSet.attribution.c_str()), html);

    // when the tile coordinate system is changed using an NSNumber
    tileSet = MGLTileSetFromTileURLTemplates(tileURLTemplates, @{
        MGLTileSourceOptionTileCoordinateSystem: @(MGLTileCoordinateSystemTMS),
    });

    // the scheme is reflected by the mbgl tileset
    XCTAssertEqual(tileSet.scheme, mbgl::Tileset::Scheme::TMS);

    // when the tile coordinate system is changed using an NSValue
    MGLTileCoordinateSystem tms = MGLTileCoordinateSystemTMS;
    tileSet = MGLTileSetFromTileURLTemplates(tileURLTemplates, @{
        MGLTileSourceOptionTileCoordinateSystem: [NSValue value:&tms withObjCType:@encode(MGLTileCoordinateSystem)],
    });

    // the scheme is reflected by the mbgl tileset
    XCTAssertEqual(tileSet.scheme, mbgl::Tileset::Scheme::TMS);
}

- (void)testInvalidTileSet {
    // a tile set that provides an mbgl tile set and invalid (crossed) minimum and maximum zoom levels throws an exception
    XCTAssertThrowsSpecificNamed(MGLTileSetFromTileURLTemplates(@[@"tile.1"], @{
        MGLTileSourceOptionMinimumZoomLevel: @10,
        MGLTileSourceOptionMaximumZoomLevel: @9,
    }), NSException, NSInvalidArgumentException);
}

@end
