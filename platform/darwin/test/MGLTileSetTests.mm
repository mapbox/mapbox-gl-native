#import <XCTest/XCTest.h>

#import <Mapbox/Mapbox.h>
#import "MGLTileSource_Private.h"

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
    
    // when the tile set has an attribution
    NSString *attribution = @"my tileset © ©️🎈";
    tileSet = MGLTileSetFromTileURLTemplates(tileURLTemplates, @{
        MGLTileSourceOptionAttribution: attribution,
    });
    
    // the attribution is reflected by the mbgl tileset
    XCTAssertEqual(tileSet.attribution, attribution.UTF8String);
    
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
