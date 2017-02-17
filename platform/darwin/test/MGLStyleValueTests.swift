import XCTest
import Mapbox

#if os(iOS) || os(watchOS) || os(tvOS)
typealias MGLColor = UIColor
#elseif os(macOS)
typealias MGLColor = NSColor
#endif
    
extension MGLStyleValueTests {
    func assertColorsEqualWithAccuracy(_ actual: MGLColor, _ expected: MGLColor, accuracy: Float = 1/255) {
        var actualComponents : [CGFloat] = [0, 0, 0, 0]
        var expectedComponents : [CGFloat] = [0, 0, 0, 0]
        actual.getRed(&(actualComponents[0]), green: &(actualComponents[1]), blue: &(actualComponents[2]), alpha: &(actualComponents[3]))
        expected.getRed(&(expectedComponents[0]), green: &(expectedComponents[1]), blue: &(expectedComponents[2]), alpha: &(expectedComponents[3]))
        for (ac, ec) in zip(actualComponents, expectedComponents) {
            XCTAssertEqualWithAccuracy(Float(ac), Float(ec), accuracy: accuracy)
        }
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
        let circleStyleLayer = MGLCircleStyleLayer(identifier: "circleLayer", source: shapeSource)
        
        // Boolean
        symbolStyleLayer.iconAllowsOverlap = MGLConstantStyleValue(rawValue: true)
        XCTAssertEqual((symbolStyleLayer.iconAllowsOverlap as! MGLConstantStyleValue<NSNumber>).rawValue, true)
        
        // Number
        symbolStyleLayer.iconHaloWidth = MGLConstantStyleValue(rawValue: 3)
        XCTAssertEqual((symbolStyleLayer.iconHaloWidth as! MGLConstantStyleValue<NSNumber>).rawValue, 3)
        
        // String
        symbolStyleLayer.text = MGLConstantStyleValue(rawValue: "{name}")
        XCTAssertEqual((symbolStyleLayer.text as! MGLConstantStyleValue<NSString>).rawValue, "{name}")

        var circleTranslationOne = CGVector(dx: 100, dy: 0)
        let circleTranslationValueOne = NSValue(bytes: &circleTranslationOne, objCType: "{CGVector=dd}")

        // non-data-driven (interpolatable property value), set to constant style value
        let expectedCircleTranslationValue = MGLStyleValue<NSValue>(rawValue: circleTranslationValueOne)
        circleStyleLayer.circleTranslation = expectedCircleTranslationValue
        XCTAssertEqual(circleStyleLayer.circleTranslation, expectedCircleTranslationValue)

        // non-data-driven (enumeration property value), set to constant style value
        let expectedCircleScaleAlignmentValue = MGLStyleValue<NSValue>(rawValue: NSValue(mglCircleScaleAlignment: .map))
        circleStyleLayer.circleScaleAlignment = expectedCircleScaleAlignmentValue
        XCTAssertEqual(circleStyleLayer.circleScaleAlignment, expectedCircleScaleAlignmentValue)
    }

    func testFunctionsWithNonDataDrivenProperties() {
        let shapeSource = MGLShapeSource(identifier: "test", shape: nil, options: nil)
        let circleStyleLayer = MGLCircleStyleLayer(identifier: "circleLayer", source: shapeSource)

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
        circleStyleLayer.circleTranslation = expectedCircleTranslationValue
        XCTAssertEqual(circleStyleLayer.circleTranslation, expectedCircleTranslationValue)

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
        circleStyleLayer.circleScaleAlignment = expectedCircleScaleAlignmentValue
        XCTAssertEqual(circleStyleLayer.circleScaleAlignment, expectedCircleScaleAlignmentValue)
    }

    func testFunctionsWithDataDrivenProperties() {
        let shapeSource = MGLShapeSource(identifier: "test", shape: nil, options: nil)
        let circleStyleLayer = MGLCircleStyleLayer(identifier: "circleLayer", source: shapeSource)
        
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
        circleStyleLayer.circleColor = expectedCircleColorValue
        assertColorValuesEqual(circleStyleLayer.circleColor as! MGLStyleFunction<MGLColor>, expectedCircleColorValue as! MGLStyleFunction<MGLColor>)
        
        // data-driven, source function with categorical color stop values with string attribute keys
        let redOnlyStops = [
            "red": MGLStyleValue<MGLColor>(rawValue: .red)
        ]
        let expectedRedCategoricalValue = MGLStyleValue<MGLColor>(
            interpolationMode: .categorical,
            sourceStops: redOnlyStops,
            attributeName: "red",
            options: [.defaultValue: MGLStyleValue<MGLColor>(rawValue: .cyan)]
        )
        circleStyleLayer.circleColor = expectedRedCategoricalValue
        assertColorValuesEqual(circleStyleLayer.circleColor, expectedRedCategoricalValue)
        
        // data-driven, source function with categorical color stop values with integer attribute keys
        let greenOrangeStops : [Float:MGLStyleValue<MGLColor>] = [
            0: MGLStyleValue<MGLColor>(rawValue: .green),
            100: MGLStyleValue<MGLColor>(rawValue: .orange)
        ]
        let expectedGreenOrangeCategoricalValue = MGLStyleValue<MGLColor>(
            interpolationMode: .categorical,
            sourceStops: greenOrangeStops,
            attributeName: "temp",
            options: [.defaultValue: MGLStyleValue<MGLColor>(rawValue: .red)]
        )
        circleStyleLayer.circleColor = expectedGreenOrangeCategoricalValue
        assertColorValuesEqual(circleStyleLayer.circleColor, expectedGreenOrangeCategoricalValue)
        
        // data-driven, source function with exponential color stop values
        let expectedRedGreenSourceExponentialValue = MGLStyleValue<MGLColor>(
            interpolationMode: .exponential,
            sourceStops: redGreenStops,
            attributeName: "temp",
            options: nil
        )
        circleStyleLayer.circleColor = expectedRedGreenSourceExponentialValue
        assertColorValuesEqual(circleStyleLayer.circleColor, expectedRedGreenSourceExponentialValue)
        
        // data-driven, identity source function
        let expectedSourceIdentityValue = MGLStyleValue<MGLColor>(
            interpolationMode: .identity,
            sourceStops: nil,
            attributeName: "size",
            options: [.defaultValue: MGLStyleValue<MGLColor>(rawValue: .green)]
        )
        circleStyleLayer.circleColor = expectedSourceIdentityValue
        assertColorValuesEqual(circleStyleLayer.circleColor, expectedSourceIdentityValue)

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
        circleStyleLayer.circleBlur = expectedCircleBlurCategoricalValue
        XCTAssertEqual(circleStyleLayer.circleBlur, expectedCircleBlurCategoricalValue)

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
        circleStyleLayer.circleRadius = expectedCompositeCategoricalValue
        XCTAssertEqual(circleStyleLayer.circleRadius, expectedCompositeCategoricalValue)

        // data-driven, composite function with inner exponential color stop values nested in outer camera stops
        let radiusCompositeExponentialOrIntervalStops: [Float: [Float: MGLStyleValue<NSNumber>]] = [
            0: [0: smallRadius],
            10: [200: smallRadius],
            20: [200: largeRadius]
        ]
        let expectedCompositeExponentialValue = MGLStyleValue<NSNumber>(
            interpolationMode: .exponential,
            compositeStops: radiusCompositeExponentialOrIntervalStops,
            attributeName: "temp",
            options: [.defaultValue: mediumRadius]
        )
        circleStyleLayer.circleRadius = expectedCompositeExponentialValue
        XCTAssertEqual(circleStyleLayer.circleRadius, expectedCompositeExponentialValue)

        // get a value back
        if let returnedCircleRadius = circleStyleLayer.circleRadius as? MGLCompositeStyleFunction<NSNumber> {
            if let returnedStops = returnedCircleRadius.stops as NSDictionary? as? [NSNumber: [NSNumber: MGLStyleValue<NSNumber>]] {
                let lhs: MGLStyleValue<NSNumber> = returnedStops[0]!.values.first!
                let rhs: MGLStyleValue<NSNumber> = radiusCompositeExponentialOrIntervalStops[0]!.values.first!
                XCTAssertEqual(lhs, rhs)
            }
        }

        // get value back as base class
        if let returnedCircleRadius = circleStyleLayer.circleRadius as? MGLStyleFunction<NSNumber> {
            if let returnedStops = returnedCircleRadius.stops as NSDictionary? as? [NSNumber: [NSNumber: MGLStyleValue<NSNumber>]] {
                let lhs: MGLStyleValue<NSNumber> = returnedStops[0]!.values.first!
                let rhs: MGLStyleValue<NSNumber> = radiusCompositeExponentialOrIntervalStops[0]!.values.first!
                XCTAssertEqual(lhs, rhs)
            }
        }

        // data-driven, composite function with inner interval color stop values nested in outer camera stops
        let expectedCompositeIntervalValue = MGLStyleValue<NSNumber>(
            interpolationMode: .interval,
            compositeStops: radiusCompositeExponentialOrIntervalStops,
            attributeName: "temp",
            options: nil
        )
        circleStyleLayer.circleRadius = expectedCompositeIntervalValue
        XCTAssertEqual(circleStyleLayer.circleRadius, expectedCompositeIntervalValue)
    }
}
