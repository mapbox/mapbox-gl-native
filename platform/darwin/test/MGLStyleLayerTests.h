#import <Mapbox/Mapbox.h>
#import <XCTest/XCTest.h>

#define MGLConstantExpression(constant) \
    [NSExpression expressionForConstantValue:constant]

@interface MGLStyleLayerTests : XCTestCase <MGLMapViewDelegate>

@property (nonatomic, copy, readonly, class) NSString *layerType;

- (void)testPropertyName:(NSString *)name isBoolean:(BOOL)isBoolean;

@end

@interface NSString (MGLStyleLayerTestAdditions)

@property (nonatomic, readonly, copy) NS_ARRAY_OF(NSString *) *lexicalClasses;
@property (nonatomic, readonly, copy) NSString *lemma;

@end

@interface NSValue (MGLStyleLayerTestAdditions)

+ (instancetype)valueWithMGLVector:(CGVector)vector;

@property (readonly) CGVector MGLVectorValue;

@end
