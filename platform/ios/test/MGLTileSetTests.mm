#import <XCTest/XCTest.h>

#import <Mapbox/Mapbox.h>
#import "MGLTileSet_Private.h"

#include <mbgl/util/tileset.hpp>

@interface MGLTileSetTests : XCTestCase

@end

@implementation MGLTileSetTests

- (void)testTileSet {
    // a tile set that provides an mbgl tile set
    MGLTileSet *tileSet = [[MGLTileSet alloc] initWithTileURLTemplates:@[@"tile.1",
                                                                         @"tile.2",
                                                                         @"tile.3"]];
    mbgl::Tileset mbglTileset = [tileSet mbglTileset];
    
    // has the correct URL templates
    XCTAssertEqual(mbglTileset.tiles.size(), 3);
    XCTAssertEqual(mbglTileset.tiles[0], "tile.1");
    XCTAssertEqual(mbglTileset.tiles[1], "tile.2");
    XCTAssertEqual(mbglTileset.tiles[2], "tile.3");
    
    // has the default scheme
    XCTAssertEqual(mbglTileset.scheme, mbgl::Tileset::Scheme::XYZ);
   
    // when the tile set has no min or max zoom level set
    tileSet.minimumZoomLevel = nil;
    tileSet.maximumZoomLevel = nil;
    
    // the mbgl object has default values for min and max zoom level
    XCTAssertEqual([tileSet mbglTileset].zoomRange.min, 0);
    XCTAssertEqual([tileSet mbglTileset].zoomRange.max, 22);
    
    // when the tile set has min and/or max zoom level set
    tileSet.minimumZoomLevel = @(1);
    tileSet.maximumZoomLevel = @(2);
    
    // the mbgl object reflects the set values for min and max zoom level
    XCTAssertEqual([tileSet mbglTileset].zoomRange.min, 1);
    XCTAssertEqual([tileSet mbglTileset].zoomRange.max, 2);
    
    // when the tile set has an attribution
    tileSet.attribution = @"my tileset © ©️🎈";
    
    // the attribution is reflected by the mbgl tileset
    XCTAssertEqual([tileSet mbglTileset].attribution, tileSet.attribution.UTF8String);
    
    // when the scheme is changed
    tileSet.scheme = MGLTileSetSchemeTMS;
    
    // the scheme is reflected by the mbgl tileset
    XCTAssertEqual([tileSet mbglTileset].scheme , mbgl::Tileset::Scheme::TMS);
    
    // a tile set that provides an mbgl tile set and minimum and maximum zoom levels
    tileSet = [[MGLTileSet alloc] initWithTileURLTemplates:@[@"tile.1"] minimumZoomLevel:15 maximumZoomLevel:20];
    
    // the zoom levels are reflected by the mbgl tileset
    XCTAssertEqual([tileSet mbglTileset].zoomRange.min, 15);
    XCTAssertEqual([tileSet mbglTileset].zoomRange.max, 20);
}

- (void)testInvalidTileSet {
    // a tile set that provides an mbgl tile set and invalid (crossed) minimum and maximum zoom levels throws an exception
    XCTAssertThrowsSpecificNamed([[MGLTileSet alloc] initWithTileURLTemplates:@[@"tile.1"] minimumZoomLevel:10 maximumZoomLevel:9], NSException,  @"Invalid minimumZoomLevel");
    
    // a tile set that provides an mbgl tile set
    MGLTileSet *tileSet = [[MGLTileSet alloc] initWithTileURLTemplates:@[@"tile.1"]];
    tileSet.maximumZoomLevel = @(10);
    
    // when the minimum zoom level is set higher than the maximum zoom level
    XCTAssertThrowsSpecificNamed(tileSet.minimumZoomLevel = @(11), NSException, @"Invalid minimumZoomLevel");
    
    // when the maximum zoom level is set lower than the minimum zoom level
    tileSet.minimumZoomLevel = @(5);
    XCTAssertThrowsSpecificNamed(tileSet.maximumZoomLevel = @(4), NSException, @"Invalid minimumZoomLevel");
}

@end
