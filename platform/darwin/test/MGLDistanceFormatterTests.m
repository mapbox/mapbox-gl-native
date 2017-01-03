#import <XCTest/XCTest.h>
#import <Mapbox/Mapbox.h>

@interface MGLDistanceFormatter(Private)
@property (nonatomic) NSNumberFormatter *numberFormatter;
@end

@interface MGLDistanceFormatterTests : XCTestCase
@end

@implementation MGLDistanceFormatterTests

- (void)testMetricAbbreviated {
    MGLDistanceFormatter *formatter = [[MGLDistanceFormatter alloc] init];
    formatter.units = MGLDistanceFormatterUnitsMetric;
    formatter.unitStyle = MGLDistanceFormatterUnitStyleAbbreviated;
    
    XCTAssertEqualObjects([formatter stringFromDistance:0], [self testStringWithDistance:0 formatter:formatter unit:@"m"]);
    XCTAssertEqualObjects([formatter stringFromDistance:10.5], [self testStringWithDistance:11 formatter:formatter unit:@"m"]);
    XCTAssertEqualObjects([formatter stringFromDistance:50], [self testStringWithDistance:50 formatter:formatter unit:@"m"]);
    XCTAssertEqualObjects([formatter stringFromDistance:500], [self testStringWithDistance:500 formatter:formatter unit:@"m"]);
    XCTAssertEqualObjects([formatter stringFromDistance:999], [self testStringWithDistance:999 formatter:formatter unit:@"m"]);
    XCTAssertEqualObjects([formatter stringFromDistance:1000], [self testStringWithDistance:1 formatter:formatter unit:@"km"]);
    XCTAssertEqualObjects([formatter stringFromDistance:1100], [self testStringWithDistance:1.1 formatter:formatter unit:@"km"]);
    XCTAssertEqualObjects([formatter stringFromDistance:1100], [self testStringWithDistance:1.1 formatter:formatter unit:@"km"]);
    XCTAssertEqualObjects([formatter stringFromDistance:32450], [self testStringWithDistance:32.5 formatter:formatter unit:@"km"]);
}

- (void)testMetric {
    MGLDistanceFormatter *formatter = [[MGLDistanceFormatter alloc] init];
    formatter.units = MGLDistanceFormatterUnitsMetric;
    formatter.unitStyle = MGLDistanceFormatterUnitStyleFull;
    
    XCTAssertEqualObjects([formatter stringFromDistance:0], [self testStringWithDistance:0 formatter:formatter unit:@"meters"]);
    XCTAssertEqualObjects([formatter stringFromDistance:1], [self testStringWithDistance:1 formatter:formatter unit:@"meter"]);
    XCTAssertEqualObjects([formatter stringFromDistance:2], [self testStringWithDistance:2 formatter:formatter unit:@"meters"]);
    XCTAssertEqualObjects([formatter stringFromDistance:500], [self testStringWithDistance:500 formatter:formatter unit:@"meters"]);
    XCTAssertEqualObjects([formatter stringFromDistance:999], [self testStringWithDistance:999 formatter:formatter unit:@"meters"]);
    XCTAssertEqualObjects([formatter stringFromDistance:1000], [self testStringWithDistance:1 formatter:formatter unit:@"kilometer"]);
    XCTAssertEqualObjects([formatter stringFromDistance:1500], [self testStringWithDistance:1.5 formatter:formatter unit:@"kilometers"]);
    XCTAssertEqualObjects([formatter stringFromDistance:2000], [self testStringWithDistance:2 formatter:formatter unit:@"kilometers"]);
}

- (void)testImperialAbbreviated {
    MGLDistanceFormatter *formatter = [[MGLDistanceFormatter alloc] init];
    formatter.units = MGLDistanceFormatterUnitsImperial;
    formatter.unitStyle = MGLDistanceFormatterUnitStyleAbbreviated;
    
    XCTAssertEqualObjects([formatter stringFromDistance:0], [self testStringWithDistance:0 formatter:formatter unit:@"ft"]);
    XCTAssertEqualObjects([formatter stringFromDistance:10], [self testStringWithDistance:33 formatter:formatter unit:@"ft"]);
    XCTAssertEqualObjects([formatter stringFromDistance:30.48], [self testStringWithDistance:100 formatter:formatter unit:@"ft"]);
    XCTAssertEqualObjects([formatter stringFromDistance:152.4], [self testStringWithDistance:500 formatter:formatter unit:@"ft"]);
    XCTAssertEqualObjects([formatter stringFromDistance:1609.34], [self testStringWithDistance:1 formatter:formatter unit:@"mi"]);
}

- (void)testImperial {
    MGLDistanceFormatter *formatter = [[MGLDistanceFormatter alloc] init];
    formatter.units = MGLDistanceFormatterUnitsImperial;
    formatter.unitStyle = MGLDistanceFormatterUnitStyleFull;
    
    XCTAssertEqualObjects([formatter stringFromDistance:0], [self testStringWithDistance:0 formatter:formatter unit:@"feet"]);
    XCTAssertEqualObjects([formatter stringFromDistance:0.3048], [self testStringWithDistance:1 formatter:formatter unit:@"foot"]);
    XCTAssertEqualObjects([formatter stringFromDistance:0.6096], [self testStringWithDistance:2 formatter:formatter unit:@"feet"]);
    XCTAssertEqualObjects([formatter stringFromDistance:304.495], [self testStringWithDistance:999 formatter:formatter unit:@"feet"]);
    XCTAssertEqualObjects([formatter stringFromDistance:1609.34], [self testStringWithDistance:1 formatter:formatter unit:@"mile"]);
    XCTAssertEqualObjects([formatter stringFromDistance:1609.34], [self testStringWithDistance:1 formatter:formatter unit:@"mile"]);
    XCTAssertEqualObjects([formatter stringFromDistance:2414.01], [self testStringWithDistance:1.5 formatter:formatter unit:@"miles"]);
    XCTAssertEqualObjects([formatter stringFromDistance:3218.68], [self testStringWithDistance:2 formatter:formatter unit:@"miles"]);
}

- (void)testImperialWithYardsAbbreviated {
    MGLDistanceFormatter *formatter = [[MGLDistanceFormatter alloc] init];
    formatter.units = MGLDistanceFormatterUnitsImperialWithYards;
    formatter.unitStyle = MGLDistanceFormatterUnitStyleAbbreviated;
    
    XCTAssertEqualObjects([formatter stringFromDistance:0], [self testStringWithDistance:0 formatter:formatter unit:@"yd"]);
    XCTAssertEqualObjects([formatter stringFromDistance:457.2], [self testStringWithDistance:500 formatter:formatter unit:@"yd"]);
    XCTAssertEqualObjects([formatter stringFromDistance:913.486], [self testStringWithDistance:999 formatter:formatter unit:@"yd"]);
    XCTAssertEqualObjects([formatter stringFromDistance:1609.34], [self testStringWithDistance:1 formatter:formatter unit:@"mi"]);
}

- (void)testImperialWithYards {
    MGLDistanceFormatter *formatter = [[MGLDistanceFormatter alloc] init];
    formatter.units = MGLDistanceFormatterUnitsImperialWithYards;
    formatter.unitStyle = MGLDistanceFormatterUnitStyleFull;
    
    XCTAssertEqualObjects([formatter stringFromDistance:0], [self testStringWithDistance:0 formatter:formatter unit:@"yards"]);
    XCTAssertEqualObjects([formatter stringFromDistance:0.9144], [self testStringWithDistance:1 formatter:formatter unit:@"yard"]);
    XCTAssertEqualObjects([formatter stringFromDistance:457.2], [self testStringWithDistance:500 formatter:formatter unit:@"yards"]);
    XCTAssertEqualObjects([formatter stringFromDistance:913.486], [self testStringWithDistance:999 formatter:formatter unit:@"yards"]);
    XCTAssertEqualObjects([formatter stringFromDistance:1609.34], [self testStringWithDistance:1 formatter:formatter unit:@"mile"]);
    XCTAssertEqualObjects([formatter stringFromDistance:2414.02], [self testStringWithDistance:1.5 formatter:formatter unit:@"miles"]);
    XCTAssertEqualObjects([formatter stringFromDistance:3218.68], [self testStringWithDistance:2 formatter:formatter unit:@"miles"]);
}

- (NSString *)testStringWithDistance:(CLLocationDistance)distance formatter:(MGLDistanceFormatter *)formatter unit:(NSString *)unit {
    return [NSString stringWithFormat:@"%@ %@", [formatter.numberFormatter numberFromString:[formatter.numberFormatter stringFromNumber:@(distance)]], unit];
}

@end
