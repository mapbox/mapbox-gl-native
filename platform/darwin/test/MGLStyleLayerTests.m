#import "MGLStyleLayerTests.h"

#import "NSString+MGLAdditions.h"

#define TEST_STRICT_NAMING_CONVENTIONS 0

@implementation MGLStyleLayerTests {
    XCTestExpectation *_styleLoadingExpectation;
}

@dynamic layerType;

- (void)setUp {
    [super setUp];
    [MGLAccountManager setAccessToken:@"pk.feedcafedeadbeefbadebede"];
    NSURL *styleURL = [[NSBundle bundleForClass:[self class]] URLForResource:@"one-liner" withExtension:@"json"];
#if TARGET_OS_IPHONE
    UIApplication *app = [UIApplication sharedApplication];
    UIViewController *vc = [[UIViewController alloc] init];
    app.keyWindow.rootViewController = vc;
    [vc view]; // Force load xib
    _mapView = [[MGLMapView alloc] initWithFrame:CGRectMake(0, 0, 256, 256) styleURL:styleURL];
    [vc.view addSubview:_mapView];
#else
    _mapView = [[MGLMapView alloc] initWithFrame:NSMakeRect(0, 0, 256, 256) styleURL:styleURL];
#endif
    _mapView.delegate = self;
    XCTAssertNil(_mapView.style);
    _styleLoadingExpectation = [self expectationWithDescription:@"Map view should finish loading style."];
    [self waitForExpectationsWithTimeout:1 handler:nil];
}

- (void)mapView:(MGLMapView *)mapView didFinishLoadingStyle:(MGLStyle *)style {
    XCTAssertNotNil(mapView.style);
    XCTAssertEqual(mapView.style, style);
    XCTAssertNil(style.name);
    
    [_styleLoadingExpectation fulfill];
}

- (void)tearDown {
    _styleLoadingExpectation = nil;
    _mapView = nil;
    
    [super tearDown];
}

- (void)testPropertyName:(NSString *)name isBoolean:(BOOL)isBoolean {
    NS_MUTABLE_ARRAY_OF(NSString *) *components = [name componentsSeparatedByString:@"-"].mutableCopy;
    if (isBoolean) {
        if ([components.firstObject isEqualToString:@"is"]) {
            [components removeObjectAtIndex:0];
            if (![components.lastObject.lexicalClasses containsObject:NSLinguisticTagAdjective]) {
                XCTAssertTrue([components.lastObject.lexicalClasses containsObject:NSLinguisticTagVerb],
                              @"Boolean getter %@ that starts with “is” should contain an adjective, past participle, or verb.", name);
                XCTAssertNotEqualObjects(components.lastObject.lemma, components.lastObject,
                                         @"Boolean getter %@ should not have infinitive, imperative, or present tense verb.", name);
            }
        } else {
            if ([components.firstObject isEqualToString:[self class].layerType]
                || [components.firstObject isEqualToString:@"icon"] || [components.firstObject isEqualToString:@"text"]) {
                [components removeObjectAtIndex:0];
            }
#if TEST_STRICT_NAMING_CONVENTIONS
            XCTAssertTrue([components.firstObject.lexicalClasses containsObject:NSLinguisticTagVerb],
                          @"Boolean getter %@ that doesn’t start with “is” should contain a verb.", name);
            XCTAssertNotEqualObjects(components.firstObject.lemma, components.lastObject);
#endif
        }
    } else {
        XCTAssertFalse([components.firstObject isEqualToString:@"is"]);
#if TEST_STRICT_NAMING_CONVENTIONS
        XCTAssertTrue([components.lastObject.lexicalClasses containsObject:NSLinguisticTagNoun],
                      @"Non-Boolean getter %@ should contain a noun.", name);
#endif
    }
}

@end

@implementation NSString (MGLStyleLayerTestAdditions)

- (NS_ARRAY_OF(NSString *) *)lexicalClasses {
    NSOrthography *orthography = [NSOrthography orthographyWithDominantScript:@"Latn"
                                                                  languageMap:@{@"Latn": @[@"en"]}];
    NSLinguisticTaggerOptions options = (NSLinguisticTaggerOmitPunctuation
                                         | NSLinguisticTaggerOmitWhitespace
                                         | NSLinguisticTaggerOmitOther);
    return [self linguisticTagsInRange:self.mgl_wholeRange
                                scheme:NSLinguisticTagSchemeLexicalClass
                               options:options
                           orthography:orthography
                           tokenRanges:NULL];
}

- (NSString *)lemma {
    NSOrthography *orthography = [NSOrthography orthographyWithDominantScript:@"Latn"
                                                                  languageMap:@{@"Latn": @[@"en"]}];
    NSLinguisticTaggerOptions options = (NSLinguisticTaggerOmitPunctuation
                                         | NSLinguisticTaggerOmitWhitespace
                                         | NSLinguisticTaggerOmitOther);
    return [self linguisticTagsInRange:self.mgl_wholeRange
                                scheme:NSLinguisticTagSchemeLemma
                               options:options
                           orthography:orthography
                           tokenRanges:NULL].firstObject;
}

@end
