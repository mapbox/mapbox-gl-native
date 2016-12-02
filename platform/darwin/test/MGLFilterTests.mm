#import "MGLStyleLayerTests.h"

#import "NSPredicate+MGLAdditions.h"
#import "MGLValueEvaluator.h"


@interface MGLFilterTests : MGLStyleLayerTests {
    MGLGeoJSONSource *source;
    MGLLineStyleLayer *layer;
}
@end

@implementation MGLFilterTests

- (void)setUp
{
    [super setUp];
    NSString *filePath = [[NSBundle bundleForClass:self.class] pathForResource:@"amsterdam" ofType:@"geojson"];
    NSURL *url = [NSURL fileURLWithPath:filePath];
    NSData *geoJSONData = [NSData dataWithContentsOfURL:url];
    source = [[MGLGeoJSONSource alloc] initWithIdentifier:@"test-source" geoJSONData:geoJSONData options:nil];
    [self.mapView.style addSource:source];
    layer = [[MGLLineStyleLayer alloc] initWithIdentifier:@"test-layer" source:source];
}

- (void)tearDown
{
    [self.mapView.style removeLayer:layer];
    [self.mapView.style removeSource:source];
}

- (NSArray<NSPredicate *> *)predicates
{
    NSPredicate *equalPredicate = [NSPredicate predicateWithFormat:@"type == 'neighbourhood'"];
    NSPredicate *notEqualPredicate = [NSPredicate predicateWithFormat:@"type != 'park'"];
    NSPredicate *greaterThanPredicate = [NSPredicate predicateWithFormat:@"%K > %@", @"stroke-width", @2.1];
    NSPredicate *greaterThanOrEqualToPredicate = [NSPredicate predicateWithFormat:@"%K >= %@", @"stroke-width", @2.1];
    NSPredicate *lessThanOrEqualToPredicate = [NSPredicate predicateWithFormat:@"%K <= %@", @"stroke-width", @2.1];
    NSPredicate *lessThanPredicate = [NSPredicate predicateWithFormat:@"%K < %@", @"stroke-width", @2.1];
    NSPredicate *inPredicate = [NSPredicate predicateWithFormat:@"type IN %@", @[@"park", @"neighbourhood"]];
    NSPredicate *notInPredicate = [NSPredicate predicateWithFormat:@"NOT (type IN %@)", @[@"park", @"neighbourhood"]];
    NSPredicate *inNotInPredicate = [NSPredicate predicateWithFormat:@"type IN %@ AND NOT (type IN %@)", @[@"park"], @[@"neighbourhood", @"test"]];
    NSPredicate *typePredicate = [NSPredicate predicateWithFormat:@"%K == %@", @"$type", @"Feature"];
    NSPredicate *idPredicate = [NSPredicate predicateWithFormat:@"%K == %@", @"$id", @"1234123"];
    NSPredicate *specialCharsPredicate = [NSPredicate predicateWithFormat:@"%K == %@", @"ty-’pè", @"sŒm-ethįng"];
    NSPredicate *booleanPredicate = [NSPredicate predicateWithFormat:@"cluster != YES"];
    NSPredicate *nilEqualsPredicate = [NSPredicate predicateWithFormat:@"type == %@", nil];
    NSPredicate *nilNotEqualsPredicate = [NSPredicate predicateWithFormat:@"type != %@", nil];
    return @[
             equalPredicate,
             notEqualPredicate,
             greaterThanPredicate,
             greaterThanOrEqualToPredicate,
             lessThanOrEqualToPredicate,
             lessThanPredicate,
             inPredicate,
             notInPredicate,
             inNotInPredicate,
             typePredicate,
             idPredicate,
             specialCharsPredicate,
             booleanPredicate,
             nilEqualsPredicate,
             nilNotEqualsPredicate
             ];
}

- (void)testAllPredicates
{
    for (NSPredicate *predicate in self.predicates) {
        layer.predicate = predicate;
        XCTAssertEqualObjects(layer.predicate, predicate);
    }
    [self.mapView.style addLayer:layer];
}

- (void)testContainsPredicate
{
    // core does not have a "contains" filter but we can achieve the equivalent by creating an `mbgl::style::InFilter`
    // and searching the value for the key
    NSPredicate *expectedPredicate = [NSPredicate predicateWithFormat:@"park IN %@", @[@"park", @"neighbourhood"]];
    NSPredicate *containsPredicate = [NSPredicate predicateWithFormat:@"%@ CONTAINS %@", @[@"park", @"neighbourhood"], @"park"];
    
    layer.predicate = containsPredicate;
    XCTAssertEqualObjects(layer.predicate, expectedPredicate);
    [self.mapView.style addLayer:layer];
}

- (void)testBetweenPredicate
{
    // core does not have a "between" filter but we can achieve the equivalent by creating a set of greater than or equal / less than or equal
    // filters for the lower and upper bounds (inclusive)
    NSPredicate *expectedPredicate = [NSCompoundPredicate predicateWithFormat:@"%K >= 2 AND %K <= 3", @"stroke-width", @"stroke-width"];
    NSPredicate *betweenPredicate = [NSPredicate predicateWithFormat:@"%K BETWEEN %@", @"stroke-width", @[@2.0, @3.0]];
    
    layer.predicate = betweenPredicate;
    XCTAssertEqualObjects(layer.predicate, expectedPredicate);
    [self.mapView.style addLayer:layer];
}

- (void)testTruePredicate
{
    // This comes out of the class cluster as an NSTruePredicate and it is equal to `[NSPredicate predicateWithValue:YES]`
    NSPredicate *truePredicate = [NSPredicate predicateWithFormat:@"TRUEPREDICATE"];
    
    layer.predicate = truePredicate;
    XCTAssertEqualObjects(layer.predicate.description, truePredicate.description);
    [self.mapView.style addLayer:layer];
}

- (void)testFalsePredicate
{
    // This comes out of the class cluster as an NSFalsePredicate and it is equal to `[NSPredicate predicateWithValue:NO]`
    NSPredicate *falsePredicate = [NSPredicate predicateWithFormat:@"FALSEPREDICATE"];
    
    layer.predicate = falsePredicate;
    XCTAssertEqualObjects(layer.predicate.description, falsePredicate.description);
    [self.mapView.style addLayer:layer];
}

- (void)testIntermittentEncoding
{
    NSPredicate *specialCharsPredicate = [NSPredicate predicateWithFormat:@"%K == %@", @"ty-’pè", @"sŒm-ethįng"];
    layer.predicate = specialCharsPredicate;
    
    NSComparisonPredicate *getPredicate = (NSComparisonPredicate *)layer.predicate;
    mbgl::style::EqualsFilter filter = layer.predicate.mgl_filter.get<mbgl::style::EqualsFilter>();
    
    id objcKey = getPredicate.leftExpression.keyPath;
    id cppKey = @(filter.key.c_str());
    id objcValue = mbgl::Value::visit(getPredicate.rightExpression.mgl_filterValue, ValueEvaluator());
    id cppValue = mbgl::Value::visit(filter.value, ValueEvaluator());
    
    XCTAssertEqualObjects(objcKey, cppKey);
    XCTAssertEqualObjects(objcValue, cppValue);
    
    [self.mapView.style addLayer:layer];
}

- (void)testNestedFilters
{
    NSPredicate *equalPredicate = [NSPredicate predicateWithFormat:@"type == 'neighbourhood'"];
    NSPredicate *notEqualPredicate = [NSPredicate predicateWithFormat:@"type != 'park'"];
    
    NSPredicate *allPredicate = [NSCompoundPredicate andPredicateWithSubpredicates:@[equalPredicate, notEqualPredicate]];
    NSPredicate *anyPredicate = [NSCompoundPredicate orPredicateWithSubpredicates:@[equalPredicate, notEqualPredicate]];
    
    layer.predicate = allPredicate;
    XCTAssertEqualObjects(layer.predicate, allPredicate);
    layer.predicate = anyPredicate;
    XCTAssertEqualObjects(layer.predicate, anyPredicate);
    
    [self.mapView.style addLayer:layer];
}

- (void)testAndPredicates
{
    NSPredicate *predicate = [NSCompoundPredicate andPredicateWithSubpredicates:self.predicates];
    layer.predicate = predicate;
    XCTAssertEqualObjects(predicate, layer.predicate);
    [self.mapView.style addLayer:layer];
}

- (void)testOrPredicates
{
    NSPredicate *predicate = [NSCompoundPredicate orPredicateWithSubpredicates:self.predicates];
    layer.predicate = predicate;
    XCTAssertEqualObjects(predicate, layer.predicate);
    [self.mapView.style addLayer:layer];
}

- (void)testNotAndPredicates
{
    NSPredicate *predicates = [NSCompoundPredicate andPredicateWithSubpredicates:self.predicates];
    NSCompoundPredicate *predicate = [NSCompoundPredicate notPredicateWithSubpredicate:predicates];
    layer.predicate = predicate;
    XCTAssertEqualObjects(predicate, layer.predicate);
    [self.mapView.style addLayer:layer];
}

- (void)testNotOrPredicates
{
    NSPredicate *predicates = [NSCompoundPredicate orPredicateWithSubpredicates:self.predicates];
    NSCompoundPredicate *predicate = [NSCompoundPredicate notPredicateWithSubpredicate:predicates];
    layer.predicate = predicate;
    XCTAssertEqualObjects(predicate, layer.predicate);
    [self.mapView.style addLayer:layer];
}

@end
