import XCTest
import Mapbox

#if os(iOS) || os(watchOS) || os(tvOS)
typealias MGLColor = UIColor
#elseif os(macOS)
typealias MGLColor = NSColor
#endif

#if swift(>=3.2)
#else
func XCTAssertEqual<T: FloatingPoint>(_ lhs: @autoclosure () throws -> T, _ rhs: @autoclosure () throws -> T, accuracy: T) {
    XCTAssertEqualWithAccuracy(lhs, rhs, accuracy: accuracy)
}
#endif
    
extension MGLStyleValueTests {
    
    struct Color {
        var red: CGFloat = 0
        var green: CGFloat = 0
        var blue: CGFloat = 0
        var alpha: CGFloat = 0
    }
    
    func assertColorsEqualWithAccuracy(_ actual: MGLColor, _ expected: MGLColor, accuracy: Float = 1/255) {
        var actualColor = Color()
        var expectedColor = Color()
        
        actual.getRed(&actualColor.red, green: &actualColor.green, blue: &actualColor.blue, alpha: &actualColor.alpha)
        expected.getRed(&expectedColor.red, green: &expectedColor.green, blue: &expectedColor.blue, alpha: &expectedColor.alpha)

        XCTAssertEqual(Float(actualColor.red), Float(expectedColor.red), accuracy: accuracy)
        XCTAssertEqual(Float(actualColor.green), Float(expectedColor.green), accuracy: accuracy)
        XCTAssertEqual(Float(actualColor.blue), Float(expectedColor.blue), accuracy: accuracy)
        XCTAssertEqual(Float(actualColor.alpha), Float(expectedColor.alpha), accuracy: accuracy)
    }
    
    func assertColorValuesEqual(_ actual: MGLStyleValue<MGLColor>, _ expected: MGLStyleValue<MGLColor>) {
        guard type(of: actual) == type(of: expected) else {
            XCTFail("Expected \(type(of: expected)), but found \(type(of: actual)) instead.")
            return
        }
        
        if let actualConstant = actual as? MGLConstantStyleValue<MGLColor> {
            assertColorsEqualWithAccuracy(actualConstant.rawValue, (expected as! MGLConstantStyleValue<MGLColor>).rawValue)
        } else if let actualFunction = actual as? MGLStyleFunction<MGLColor>,
            let expectedFunction = expected as? MGLStyleFunction<MGLColor> {
            
            // unless we have stops, there's no need for a custom comparison - default to plain == assertion
            guard let actualStops = actualFunction.stops, let expectedStops = expectedFunction.stops else {
                XCTAssertEqual(actualFunction, expectedFunction)
                return
            }
            
            guard expectedStops is [String: Any] || expectedStops is [Float:Any] else {
                XCTFail("Stop levels must be String or Float.")
                return
            }
            
            XCTAssertEqual(actualFunction.interpolationBase, expectedFunction.interpolationBase)
            XCTAssertEqual(actualFunction.interpolationMode, expectedFunction.interpolationMode)
            if let actualFunction = actualFunction as? MGLSourceStyleFunction<MGLColor>,
                let expectedFunction = expectedFunction as? MGLSourceStyleFunction<MGLColor> {
                XCTAssertEqual(actualFunction.defaultValue, expectedFunction.defaultValue)
            } else if let actualFunction = actualFunction as? MGLCompositeStyleFunction<MGLColor>,
                let expectedFunction = expectedFunction as? MGLCompositeStyleFunction<MGLColor> {
                XCTAssertEqual(actualFunction.defaultValue, expectedFunction.defaultValue)
            }
            
            func assertStopEqual (_ actualValue: Any?, _ expectedValue: Any?) {
                guard type(of: actualValue) == type(of: expectedValue) else {
                    XCTFail("Expected stop value of type \(type(of: expectedValue)), but found \(type(of: actualValue)) instead.")
                    return
                }
                if let actualValue = actualValue as? MGLConstantStyleValue<MGLColor>,
                    let expectedValue = expectedValue as? MGLConstantStyleValue<MGLColor> {
                    assertColorsEqualWithAccuracy(actualValue.rawValue, expectedValue.rawValue)
                } else if let actualValue = actualValue as? MGLConstantStyleValue<AnyObject>,
                    let expectedValue = expectedValue as? MGLConstantStyleValue<AnyObject> {
                    XCTAssertEqual(actualValue, expectedValue)
                } else {
                    XCTFail("Unsupported stop value type \(type(of: actualValue)).")
                }
            }
            
            XCTAssertEqual(actualStops.count, expectedStops.count)
            if let actualStops = actualStops as? [String:Any], let expectedStops = expectedStops as? [String: Any] {
                for (key, value) in actualStops {
                    assertStopEqual(value, expectedStops[key])
                }
            } else if let actualStops = actualStops as? [Float:Any], let expectedStops = expectedStops as? [Float:Any] {
                for (key, value) in actualStops {
                    assertStopEqual(value, expectedStops[key])
                }
            } else {
                XCTFail("Expected stops of type \(type(of: Array(expectedStops.keys)).Index.self), but found \(type(of: Array(actualStops.keys)).Index.self) instead.")
                return
            }
        } else {
            XCTFail("MGLStyleValue<MGLColor> must be either a constant or a style function.")
        }
    }
    
    func testConstantValues() {
        let shapeSource = MGLShapeSource(identifier: "source", shape: nil, options: nil)
        let symbolStyleLayer = MGLSymbolStyleLayer(identifier: "symbolLayer", source: shapeSource)
        _ = MGLCircleStyleLayer(identifier: "circleLayer", source: shapeSource)
        
        // Boolean
        let iconAllowsOverlap = MGLConstantStyleValue(rawValue: true as NSNumber)
        XCTAssertEqual(iconAllowsOverlap.rawValue, true)
        
        // Number
        let iconHaloWidth = MGLConstantStyleValue(rawValue: 3 as NSNumber)
        XCTAssertEqual(iconHaloWidth.rawValue, 3)
        
        // String
        symbolStyleLayer.text = NSExpression(forKeyPath: "name")
        XCTAssertEqual(symbolStyleLayer.text, NSExpression(forKeyPath: "name"))

        var circleTranslationOne = CGVector(dx: 100, dy: 0)
        let circleTranslationValueOne = NSValue(bytes: &circleTranslationOne, objCType: "{CGVector=dd}")

        // non-data-driven (interpolatable property value), set to constant style value
        let expectedCircleTranslationValue = MGLStyleValue<NSValue>(rawValue: circleTranslationValueOne)
        let circleTranslation = expectedCircleTranslationValue
        XCTAssertEqual(circleTranslation, expectedCircleTranslationValue)

        // non-data-driven (enumeration property value), set to constant style value
        let expectedCircleScaleAlignmentValue = MGLStyleValue<NSValue>(rawValue: NSValue(mglCircleScaleAlignment: .map))
        let circleScaleAlignment = expectedCircleScaleAlignmentValue
        XCTAssertEqual(circleScaleAlignment, expectedCircleScaleAlignmentValue)
    }

    func testFunctionsWithNonDataDrivenProperties() {
        let shapeSource = MGLShapeSource(identifier: "test", shape: nil, options: nil)
        _ = MGLCircleStyleLayer(identifier: "circleLayer", source: shapeSource)

        var circleTranslationOne = CGVector(dx: 100, dy: 0)
        let circleTranslationValueOne = NSValue(bytes: &circleTranslationOne, objCType: "{CGVector=dd}")
        var circleTranslationTwo = CGVector(dx: 0, dy: 0)
        let circleTranslationValueTwo = NSValue(bytes: &circleTranslationTwo, objCType: "{CGVector=dd}")

        let circleTranslationStops : [Float:MGLStyleValue<NSValue>] = [
            0: MGLStyleValue<NSValue>(rawValue: circleTranslationValueOne),
            10: MGLStyleValue<NSValue>(rawValue: circleTranslationValueTwo)
        ]

        // non-data-driven (interpolatable property value), camera function with CGVector (NSValue) stop values
        let expectedCircleTranslationValue = MGLStyleValue<NSValue>(
            interpolationMode: .interval,
            cameraStops: circleTranslationStops,
            options: nil
        )
        let circleTranslation = expectedCircleTranslationValue
        XCTAssertEqual(circleTranslation, expectedCircleTranslationValue)

        // non-data-driven (enumeration property value), camera function with MGLCircleScaleAlignment enum (NSValue) stop values
        let scaleAlignmentStops : [Float:MGLStyleValue<NSValue>] = [
            0: MGLStyleValue(rawValue: NSValue(mglCircleScaleAlignment: .map)),
            10: MGLStyleValue(rawValue: NSValue(mglCircleScaleAlignment: .viewport))
        ]
        let expectedCircleScaleAlignmentValue = MGLStyleValue<NSValue>(
            interpolationMode: .interval,
            cameraStops: scaleAlignmentStops,
            options: nil
        )
        let circleScaleAlignment = expectedCircleScaleAlignmentValue
        XCTAssertEqual(circleScaleAlignment, expectedCircleScaleAlignmentValue)
    }

    func testFunctionsWithDataDrivenProperties() {
        let shapeSource = MGLShapeSource(identifier: "test", shape: nil, options: nil)
        _ = MGLCircleStyleLayer(identifier: "circleLayer", source: shapeSource)
        
        // data-driven, camera function with exponential color stop values
        let redGreenStops : [Float:MGLStyleValue<MGLColor>] = [
            0: MGLStyleValue<MGLColor>(rawValue: .red),
            10: MGLStyleValue<MGLColor>(rawValue: .red),
            15: MGLStyleValue<MGLColor>(rawValue: .green)
        ]
        let expectedCircleColorValue = MGLStyleValue<MGLColor>(
            interpolationMode: .exponential,
            cameraStops: redGreenStops,
            options: [.interpolationBase: 10.0]
        )
        let circleColor = expectedCircleColorValue
        assertColorValuesEqual(circleColor as! MGLStyleFunction<MGLColor>, expectedCircleColorValue as! MGLStyleFunction<MGLColor>)
        
        // data-driven, source function with categorical color stop values with string attribute keys
        let redOnlyStops = [
            "red": MGLStyleValue<MGLColor>(rawValue: .red)
        ]
        _ = MGLStyleValue<MGLColor>(
            interpolationMode: .categorical,
            sourceStops: redOnlyStops,
            attributeName: "red",
            options: [.defaultValue: MGLStyleValue<MGLColor>(rawValue: .cyan)]
        )
        
        // data-driven, source function with categorical color stop values with integer attribute keys
        let greenOrangeStops : [Float:MGLStyleValue<MGLColor>] = [
            0: MGLStyleValue<MGLColor>(rawValue: .green),
            100: MGLStyleValue<MGLColor>(rawValue: .orange)
        ]
        _ = MGLStyleValue<MGLColor>(
            interpolationMode: .categorical,
            sourceStops: greenOrangeStops,
            attributeName: "temp",
            options: [.defaultValue: MGLStyleValue<MGLColor>(rawValue: .red)]
        )
        
        // data-driven, source function with exponential color stop values
        _ = MGLStyleValue<MGLColor>(
            interpolationMode: .exponential,
            sourceStops: redGreenStops,
            attributeName: "temp",
            options: nil
        )
        
        // data-driven, identity source function
        let _ = MGLStyleValue<MGLColor>(
            interpolationMode: .identity,
            sourceStops: nil,
            attributeName: "size",
            options: [.defaultValue: MGLStyleValue<MGLColor>(rawValue: .green)]
        )

        // data-driven, source function with categorical color stop values with boolean attribute keys
        let booleanCategoricalStops = [
            false: MGLStyleValue<NSNumber>(rawValue: 0),
            true: MGLStyleValue<NSNumber>(rawValue: 2)
        ]
        let expectedCircleBlurCategoricalValue = MGLStyleValue<NSNumber>(
            interpolationMode: .categorical,
            sourceStops: booleanCategoricalStops,
            attributeName: "fuzzy",
            options: [.defaultValue: MGLStyleValue<NSNumber>(rawValue: 42)]
        )
        let circleBlur = expectedCircleBlurCategoricalValue
        XCTAssertEqual(circleBlur, expectedCircleBlurCategoricalValue)

        // data-driven, composite function with inner categorical color stop values with string attribute keys nested in outer camera stops
        let smallRadius = MGLStyleValue<NSNumber>(rawValue: 5)
        let mediumRadius = MGLStyleValue<NSNumber>(rawValue: 10)
        let largeRadius = MGLStyleValue<NSNumber>(rawValue: 20)
        let radiusCompositeCategoricalStops: [Float: [String: MGLStyleValue<NSNumber>]] = [
            0: ["green": smallRadius],
            10: ["green": smallRadius],
            15: ["green": largeRadius],
            20: ["green": largeRadius]
        ]
        let defaultRadius = MGLStyleValue<NSNumber>(rawValue: 2)
        let expectedCompositeCategoricalValue = MGLStyleValue<NSNumber>(
            interpolationMode: .categorical,
            compositeStops: radiusCompositeCategoricalStops,
            attributeName: "color",
            options: [.defaultValue: defaultRadius]
        )
        let circleRadius = expectedCompositeCategoricalValue
        
        var compositeValue = circleRadius as! MGLCompositeStyleFunction
        var expectedCompositeValue = expectedCompositeCategoricalValue as! MGLCompositeStyleFunction
        XCTAssertEqual(compositeValue.attributeName, expectedCompositeValue.attributeName)
        XCTAssertEqual(compositeValue.stops as? [Float: [String: MGLStyleValue<NSNumber>]] as NSDictionary? ?? [:], radiusCompositeCategoricalStops as NSDictionary)
        XCTAssertEqual(compositeValue.interpolationMode, expectedCompositeValue.interpolationMode)
        XCTAssertEqual(compositeValue.defaultValue, expectedCompositeValue.defaultValue)

        // data-driven, composite function with inner exponential color stop values nested in outer camera stops
        let radiusCompositeExponentialOrIntervalStops: [Float: [Float: MGLStyleValue<NSNumber>]] = [
            0: [0: MGLStyleValue<NSNumber>(rawValue: 5)],
            10: [200: MGLStyleValue<NSNumber>(rawValue: 5)],
            20: [200: MGLStyleValue<NSNumber>(rawValue: 20)]
        ]

        let expectedStops = [
            0: [0: MGLStyleValue<NSNumber>(rawValue: 5)],
            10: [200: MGLStyleValue<NSNumber>(rawValue: 5)],
            20: [200: MGLStyleValue<NSNumber>(rawValue: 20)]
        ]
        compositeValue = MGLStyleValue<NSNumber>(
            interpolationMode: .exponential,
            compositeStops: [
                0: [0: MGLStyleValue<NSNumber>(rawValue: 5)],
                10: [200: MGLStyleValue<NSNumber>(rawValue: 5)],
                20: [200: MGLStyleValue<NSNumber>(rawValue: 20)]
            ],
            attributeName: "temp",
            options: [.defaultValue: mediumRadius]
            ) as! MGLCompositeStyleFunction
        
        let expectedCompositeExponentialValue = MGLStyleValue<NSNumber>(
            interpolationMode: .exponential,
            compositeStops: radiusCompositeExponentialOrIntervalStops,
            attributeName: "temp",
            options: [.defaultValue: mediumRadius]
        )

        expectedCompositeValue = expectedCompositeExponentialValue as! MGLCompositeStyleFunction
        XCTAssertEqual(compositeValue.attributeName, expectedCompositeValue.attributeName)
        XCTAssertEqual(compositeValue.stops as NSDictionary, expectedStops as NSDictionary)
        XCTAssertEqual(compositeValue.interpolationMode, expectedCompositeValue.interpolationMode)
        XCTAssertEqual(compositeValue.defaultValue, expectedCompositeValue.defaultValue)

        // get a value back
        if let returnedCircleRadius = circleRadius as? MGLCompositeStyleFunction<NSNumber> {
            if let returnedStops = returnedCircleRadius.stops as? [Float: [Float: MGLStyleValue<NSNumber>]] {
                let lhs: MGLStyleValue<NSNumber> = returnedStops[0]!.values.first!
                let rhs: MGLStyleValue<NSNumber> = radiusCompositeExponentialOrIntervalStops[0]!.values.first!
                XCTAssertEqual(lhs, rhs)
            }
        }

        // get value back as base class
        if let returnedCircleRadius = circleRadius as? MGLStyleFunction<NSNumber> {
            if let returnedStops = returnedCircleRadius.stops as NSDictionary? as? [NSNumber: [NSNumber: MGLStyleValue<NSNumber>]] {
                let lhs: MGLStyleValue<NSNumber> = returnedStops[0]!.values.first!
                let rhs: MGLStyleValue<NSNumber> = radiusCompositeExponentialOrIntervalStops[0]!.values.first!
                XCTAssertEqual(lhs, rhs)
            }
        }

        // data-driven, composite function with inner interval color stop values nested in outer camera stops
        let expectedCompositeIntervalValue = MGLStyleValue<NSNumber>(
            interpolationMode: .interval,
            compositeStops: [
                
                10: [200: MGLStyleValue<NSNumber>(rawValue: 5)],
                20: [200: MGLStyleValue<NSNumber>(rawValue: 20)]
            ],
            attributeName: "temp",
            options: nil
        )
        compositeValue = MGLStyleValue<NSNumber>(
            interpolationMode: .interval,
            compositeStops: [
                0: [0: MGLStyleValue<NSNumber>(rawValue: 5)],
                10: [200: MGLStyleValue<NSNumber>(rawValue: 5)],
                20: [200: MGLStyleValue<NSNumber>(rawValue: 20)]
            ],
            attributeName: "temp",
            options: nil
        ) as! MGLCompositeStyleFunction
        
        expectedCompositeValue = expectedCompositeIntervalValue as! MGLCompositeStyleFunction
        XCTAssertEqual(compositeValue.attributeName, expectedCompositeValue.attributeName)
        XCTAssertEqual(compositeValue.stops as NSDictionary, expectedStops as NSDictionary)
        XCTAssertEqual(compositeValue.interpolationMode, expectedCompositeValue.interpolationMode)
        XCTAssertEqual(compositeValue.defaultValue, expectedCompositeValue.defaultValue)
    }
}
