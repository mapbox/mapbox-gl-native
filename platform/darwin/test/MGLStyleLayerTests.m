#import "MGLStyleLayerTests.h"

#import "NSString+MGLAdditions.h"

#define TEST_STRICT_NAMING_CONVENTIONS 0

@implementation MGLStyleLayerTests

@dynamic layerType;

- (void)testProperties {
    MGLPointFeature *feature = [[MGLPointFeature alloc] init];
    MGLShapeSource *source = [[MGLShapeSource alloc] initWithIdentifier:@"sourceID" shape:feature options:nil];

    MGLFillStyleLayer *layer = [[MGLFillStyleLayer alloc] initWithIdentifier:@"layerID" source:source];

    XCTAssertEqualObjects(layer.identifier, @"layerID");
    XCTAssertEqualObjects(layer.sourceIdentifier, source.identifier);

    XCTAssertTrue(layer.visible);
    layer.visible = NO;
    XCTAssertFalse(layer.visible);
    layer.visible = YES;
    XCTAssertTrue(layer.visible);

    XCTAssertEqual(layer.minimumZoomLevel, -INFINITY);
    layer.minimumZoomLevel = 22;
    XCTAssertEqual(layer.minimumZoomLevel, 22);

    XCTAssertEqual(layer.maximumZoomLevel, INFINITY);
    layer.maximumZoomLevel = 0;
    XCTAssertEqual(layer.maximumZoomLevel, 0);
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

@implementation NSValue (MGLStyleLayerTestAdditions)

+ (instancetype)valueWithMGLVector:(CGVector)vector {
#if TARGET_OS_IPHONE
    return [self valueWithCGVector:vector];
#else
    return [self value:&vector withObjCType:@encode(CGVector)];
#endif
}

- (CGVector)MGLVectorValue {
#if TARGET_OS_IPHONE
    return self.CGVectorValue;
#else
    CGVector vector;
    [self getValue:&vector];
    return vector;
#endif
}

@end
