#import "MGLMapViewIntegrationTest.h"
#import "../../darwin/src/MGLSignpost.h"

@interface MGLMapView (MBXViewController)
- (UIEdgeInsets)defaultEdgeInsetsForShowAnnotations;
@end

@interface MGLStyle (MBXViewController)
@property (nonatomic, readonly, copy) NSArray<MGLVectorStyleLayer *> *roadStyleLayers;
@end

NSDictionary* MGLTestAccessibilityDictionaryForElement(NSObject* element);
NSDictionary *readAccessibilityDictionaryFromBundle(NSString *filename);
void writeDictionaryToFile(NSDictionary *info, NSString *filename);

@interface MGLQueryFeatureTests : MGLMapViewIntegrationTest
@end

@implementation MGLQueryFeatureTests

- (void)setupAccessToken {
    [self validAccessToken];
}

- (NSURL*)styleURL {
    return [MGLStyle streetsStyleURL];
}

- (void)setUp {
    [super setUp];
    
    NSArray *annotations = [self viewAnnotationsAroundDC:100];
    
    [self.mapView addAnnotations:annotations];
    UIEdgeInsets insets = [self.mapView defaultEdgeInsetsForShowAnnotations];
    [self.mapView showAnnotations:annotations
                      edgePadding:insets
                         animated:NO
                completionHandler:nil];
    
    // wait for idle
    [self waitForMapViewToIdleWithTimeout:10.0];
}

- (void)testQueryRoadsPerformanceAroundDC {
    if (![self validAccessToken]) {
        return;
    }
    
    os_signpost_id_t signpost = MGL_CREATE_SIGNPOST();
    MGL_SIGNPOST_BEGIN(signpost, "testQueryRoadsAroundDC");
    [self measureBlock:^{
        [self queryRoads];
    }];
    MGL_SIGNPOST_END(signpost, "testQueryRoadsAroundDC");
}

- (void)testQueryRoadsAroundDCAccessibilityResults {
    if(![NSProcessInfo.processInfo isOperatingSystemAtLeastVersion:(NSOperatingSystemVersion){11,0,0}]) {
        printf("warning: This test requires iOS 11+ - skipping\n");
        return;
    }
    
    [self queryRoads];
    
    NSDictionary *accessibilityDictionary = MGLTestAccessibilityDictionaryForElement(self.mapView);
    
    NSString *filename = NSStringFromSelector(_cmd);

#if 0
    writeDictionaryToFile(accessibilityDictionary, [NSString stringWithFormat:@"%@.json", filename]);
#endif
    
    NSDictionary *expected = readAccessibilityDictionaryFromBundle(filename);
    XCTAssertNotNil(expected);
    [self assertAccessibilityDictionary:accessibilityDictionary isEqualToDictionary:expected];
}

#pragma mark - Common methods

- (NSArray*)viewAnnotationsAroundDC:(NSUInteger)count
{
    NSData *featuresData = [NSData dataWithContentsOfFile:[[NSBundle mainBundle] pathForResource:@"points" ofType:@"geojson"]];
    NSDictionary *features = [NSJSONSerialization JSONObjectWithData:featuresData
                                                             options:0
                                                               error:nil];
    
    XCTAssert([features isKindOfClass:[NSDictionary class]]);
    NSMutableArray *annotations = [NSMutableArray array];
    
    for (NSDictionary *feature in features[@"features"])
    {
        CLLocationCoordinate2D coordinate = CLLocationCoordinate2DMake([feature[@"geometry"][@"coordinates"][1] doubleValue],
                                                                       [feature[@"geometry"][@"coordinates"][0] doubleValue]);
        NSString *title = feature[@"properties"][@"NAME"];
        
        MGLPointAnnotation *annotation = [MGLPointAnnotation new];
        
        annotation.coordinate = coordinate;
        annotation.title = title;
        
        [annotations addObject:annotation];
        
        if (annotations.count == count) {
            break;
        }
    }
    return annotations;
}

- (void)queryRoads
{
    os_signpost_id_t signpost = MGL_CREATE_SIGNPOST();

    MGL_SIGNPOST_BEGIN(signpost, "query-roads");
    NSArray *roadStyleLayerIdentifiers = [self.mapView.style.roadStyleLayers valueForKey:@"identifier"];
    XCTAssertNotNil(roadStyleLayerIdentifiers);
    NSArray *visibleRoadFeatures = [self.mapView visibleFeaturesInRect:self.mapView.bounds inStyleLayersWithIdentifiers:[NSSet setWithArray:roadStyleLayerIdentifiers]];
    MGL_SIGNPOST_END(signpost, "query-roads", "%lu", (unsigned long)visibleRoadFeatures.count);

    XCTAssertNotNil(visibleRoadFeatures);
    XCTAssert(visibleRoadFeatures.count > 0);
    
    NSLog(@"Roads & labels feature count: %lu", (unsigned long)visibleRoadFeatures.count);
}

#pragma mark - Accessibility utilities

- (void)assertAccessibilityDictionary:(NSDictionary*)a isEqualToDictionary:(NSDictionary*)b
{
    NSArray *keys = @[@"isElement", @"ElementsHidden", @"Label", @"Hint", @"Value", @"Traits"];
    
    for (NSString *key in keys) {
        XCTAssertEqualObjects(a[key], b[key]);
    }
    
    [self assertFloatArray:a[@"Frame"] isEqualToFloatArray:b[@"Frame"] accuracy:0.001];
    [self assertBezierPathArray:a[@"Path"] isEqualToBezierPathArray:b[@"Path"] accuracy:0.001];
    
    NSArray *aElements = a[@"Elements"];
    NSArray *bElements = b[@"Elements"];
    
    XCTAssertEqual(aElements.count, bElements.count);
    if (aElements.count != bElements.count) {
        return;
    }
    
    for (NSUInteger i = 0; i < aElements.count; i++) {
        NSDictionary *aElement = aElements[i];
        NSDictionary *bElement = bElements[i];

        [self assertAccessibilityDictionary:aElement isEqualToDictionary:bElement];
    }
}

- (void)assertFloatArray:(NSArray *)a isEqualToFloatArray:(NSArray *)b accuracy:(CGFloat)accuracy
{
    XCTAssertEqual(a.count, b.count);
    if (a.count != b.count) {
        return;
    }

    for (NSUInteger i = 0; i < a.count; i++)
    {
        XCTAssertEqualWithAccuracy(((NSNumber*)a[i]).floatValue, ((NSNumber*)b[i]).floatValue, accuracy);
    }
}

- (void)assertBezierPathArray:(NSArray *)a isEqualToBezierPathArray:(NSArray *)b accuracy:(CGFloat)accuracy
{
    XCTAssertEqual(a.count, b.count);
    if (a.count != b.count) {
        return;
    }
    
    for (NSUInteger i = 0; i < a.count; i++) {
        NSDictionary *adict = a[i];
        NSDictionary *bdict = b[i];
        
        NSString *aKey = adict.allKeys.firstObject;
        NSString *bKey = bdict.allKeys.firstObject;
        
        XCTAssertNotNil(aKey);
        XCTAssertNotNil(bKey);
        XCTAssertEqualObjects(aKey, bKey);
        
        NSArray *aValues = adict[aKey];
        NSArray *bValues = bdict[bKey];
        
        [self assertFloatArray:aValues isEqualToFloatArray:bValues accuracy:accuracy];
    }
}
@end

void writeDictionaryToFile(NSDictionary *info, NSString *filename)
{
    NSData *jsondata = [NSJSONSerialization dataWithJSONObject:info options:NSJSONWritingPrettyPrinted error:NULL];
    NSString *path = [[[[NSFileManager defaultManager] URLsForDirectory:NSDocumentDirectory
                                                              inDomains:NSUserDomainMask] lastObject].path
                      stringByAppendingPathComponent:filename];
    [jsondata writeToFile:path atomically:YES];
    NSLog(@"Wrote to path = %@", path);
}

NSDictionary *readAccessibilityDictionaryFromBundle(NSString *filename)

{
    NSString *path = [[NSBundle mainBundle] pathForResource:filename ofType:@"json"];
    NSData *jsonData = [NSData dataWithContentsOfFile:path];
    NSLog(@"Read from path = %@", path);

    NSDictionary *info = [NSJSONSerialization JSONObjectWithData:jsonData options:0 error:NULL];
    return info;
}

NSArray* MGLTestArrayForCGRect(CGRect rect)
{
    return @[
             @(rect.origin.x),
             @(rect.origin.y),
             @(rect.size.width),
             @(rect.size.height)
             ];
}

NSArray* MGLTestArrayForBezierPath(UIBezierPath* path)
{
    if (!path) {
        return @[];
    }
    
    CGPathRef pathRef = path.CGPath;
    NSMutableArray *elements = [NSMutableArray array];
    
    if (@available(iOS 11.0, *)) {
        CGPathApplyWithBlock(pathRef, ^(const CGPathElement * _Nonnull element) {
            // We're currently only using move-to & line-to so only use the first
            // element point.
            NSDictionary *elementDict = @{
                                          [NSString stringWithFormat:@"%d", element->type] : element->points ?
                                          @[@(element->points->x),@(element->points->y)] : @[]
                                          };
            [elements addObject:elementDict];
        });
    }
    
    return elements;
}

NSDictionary* MGLTestAccessibilityDictionaryForElement(NSObject* element)
{
    NSInteger elementCount = 0;
    NSArray *elements = @[];

    // Recurse
    elementCount = 0;
    
    // Looks like element count is 0x7fff... if not a container.
    if (element.accessibilityElementCount != LONG_MAX) {
        elementCount = element.accessibilityElementCount;
        NSMutableArray *mutableElements = [NSMutableArray arrayWithCapacity:elementCount];
        for (NSInteger i = 0; i<elementCount; i++) {
            NSObject *sub = [element accessibilityElementAtIndex:i];
            NSDictionary *dict = MGLTestAccessibilityDictionaryForElement(sub);
            [mutableElements addObject:dict];
        }
        elements = mutableElements;
    }
    
    NSDictionary *result =
    @{
      @"isElement" : @(element.isAccessibilityElement),
      @"Label"     : element.accessibilityLabel ?: @"NULL",
      @"Hint"      : element.accessibilityHint  ?: @"NULL",
      @"Value"     : element.accessibilityValue ?: @"NULL",
      @"Traits"    : @(element.accessibilityTraits),
      @"Frame"     : MGLTestArrayForCGRect(element.accessibilityFrame),
      @"Path"      : MGLTestArrayForBezierPath(element.accessibilityPath),
      @"ElementsHidden" : @(element.accessibilityElementsHidden),
      @"Elements" : elements,
      };
    
    return result;
}
