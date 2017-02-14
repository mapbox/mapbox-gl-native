import XCTest
import Mapbox

#if os(iOS) || os(watchOS) || os(tvOS)
typealias ColorType = UIColor
#elseif os(macOS)
typealias ColorType = NSColor
#endif
    
extension MGLStyleValueTests {
    func assertColorsEqualWithAccuracy(_ actual: ColorType, _ expected: ColorType, accuracy: Float = 0.1) {
        var actualComponents : [CGFloat] = [0, 0, 0, 0]
        var expectedComponents : [CGFloat] = [0, 0, 0, 0]
        actual.getRed(&(actualComponents[0]), green: &(actualComponents[1]), blue: &(actualComponents[2]), alpha: &(actualComponents[3]))
        expected.getRed(&(expectedComponents[0]), green: &(expectedComponents[1]), blue: &(expectedComponents[2]), alpha: &(expectedComponents[3]))
        for (ac, ec) in zip(actualComponents, expectedComponents) {
            XCTAssertEqualWithAccuracy(Float(ac), Float(ec), accuracy: accuracy)
        }
    }
    
    func assertColorValuesEqual(_ actual: MGLStyleValue<ColorType>, _ expected: MGLStyleValue<ColorType>) {
        if let actualConstant = actual as? MGLStyleConstantValue<ColorType> {
            XCTAssertTrue(expected is MGLStyleConstantValue<ColorType>)
            assertColorsEqualWithAccuracy(actualConstant.rawValue, (expected as! MGLStyleConstantValue<ColorType>).rawValue)
            return
        } else if let actualFunction = actual as? MGLStyleFunction<ColorType> {
            XCTAssertTrue(expected is MGLStyleFunction<ColorType>)
            let expectedFunction = expected as! MGLStyleFunction<ColorType>
            XCTAssertEqual(actualFunction.interpolationBase, expectedFunction.interpolationBase)
            XCTAssertEqual(actualFunction.interpolationMode, expectedFunction.interpolationMode)
            // TODO: assert default values are equal
            if actualFunction.stops == nil {
                XCTAssertNil(expectedFunction.stops)
                return
            }
            let actualStops = actualFunction.stops!
            let expectedStops = expectedFunction.stops!
            XCTAssertEqual(actualStops.keys.count, expectedStops.keys.count)
            for (stopKey, stopValue) in actualStops {
                if let actualValue = stopValue as? NSNumber {
                    let expectedValue = expectedStops[stopKey] as? NSNumber
                    XCTAssertEqual(actualValue, expectedValue)
                } else if let actualValue = stopValue as? ColorType {
                    let expectedValue = expectedStops[stopKey] as? ColorType
                    XCTAssertNotNil(expectedValue)
                    assertColorsEqualWithAccuracy(actualValue, expectedValue!)
                }
            }
        }
    }
    
    func testConstantValues() {
        let shapeSource = MGLShapeSource(identifier: "source", shape: nil, options: nil)
        let symbolStyleLayer = MGLSymbolStyleLayer(identifier: "symbolLayer", source: shapeSource)
        let circleStyleLayer = MGLCircleStyleLayer(identifier: "circleLayer", source: shapeSource)
        
        // Boolean
        symbolStyleLayer.iconAllowsOverlap = MGLStyleConstantValue(rawValue: true)
        XCTAssertEqual((symbolStyleLayer.iconAllowsOverlap as! MGLStyleConstantValue<NSNumber>).rawValue, true)
        
        // Number
        symbolStyleLayer.iconHaloWidth = MGLStyleConstantValue(rawValue: 3)
        XCTAssertEqual((symbolStyleLayer.iconHaloWidth as! MGLStyleConstantValue<NSNumber>).rawValue, 3)
        
        // String
        symbolStyleLayer.text = MGLStyleConstantValue(rawValue: "{name}")
        XCTAssertEqual((symbolStyleLayer.text as! MGLStyleConstantValue<NSString>).rawValue, "{name}")

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

        let circleTranslationStops = [
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
        let scaleAlignmentStops = [
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

        #if os(iOS) || os(watchOS) || os(tvOS)

            // data-driven, camera function with exponential color stop values
            let redGreenStops = [
                0: MGLStyleValue<UIColor>(rawValue: .red),
                10: MGLStyleValue<UIColor>(rawValue: .red),
                15: MGLStyleValue<UIColor>(rawValue: .green)
            ]
            let expectedCircleColorValue = MGLStyleValue<UIColor>(
                interpolationMode: .exponential,
                cameraStops: redGreenStops,
                options: [.interpolationBase: 10.0]
            )
            circleStyleLayer.circleColor = expectedCircleColorValue
            assertColorValuesEqual(circleStyleLayer.circleColor as! MGLStyleFunction<UIColor>, expectedCircleColorValue as! MGLStyleFunction<UIColor>)

            // data-driven, source function with categorical color stop values with string attribute keys
            let redOnlyStops = [
                "red": MGLStyleValue<UIColor>(rawValue: .red)
            ]
            let expectedRedCategoricalValue = MGLStyleValue<UIColor>(
                interpolationMode: .categorical,
                sourceStops: redOnlyStops,
                attributeName: "red",
                options: [.defaultValue: MGLStyleValue<UIColor>(rawValue: .cyan)]
            )
            circleStyleLayer.circleColor = expectedRedCategoricalValue
            assertColorValuesEqual(circleStyleLayer.circleColor, expectedRedCategoricalValue)

            // data-driven, source function with categorical color stop values with integer attribute keys
            let greenOrangeStops = [
                0: MGLStyleValue<UIColor>(rawValue: .green),
                100: MGLStyleValue<UIColor>(rawValue: .orange)
            ]
            let expectedGreenOrangeCategoricalValue = MGLStyleValue<UIColor>(
                interpolationMode: .categorical,
                sourceStops: greenOrangeStops,
                attributeName: "temp",
                options: [.defaultValue: MGLStyleValue<UIColor>(rawValue: .red)]
            )
            circleStyleLayer.circleColor = expectedGreenOrangeCategoricalValue
            assertColorValuesEqual(circleStyleLayer.circleColor, expectedGreenOrangeCategoricalValue)

            // data-driven, source function with exponential color stop values
            let expectedRedGreenSourceExponentialValue = MGLStyleValue<UIColor>(
                interpolationMode: .exponential,
                sourceStops: redGreenStops,
                attributeName: "temp",
                options: nil
            )
            circleStyleLayer.circleColor = expectedRedGreenSourceExponentialValue
            assertColorValuesEqual(circleStyleLayer.circleColor, expectedRedGreenSourceExponentialValue)

            // data-driven, identity source function
            let expectedSourceIdentityValue = MGLStyleValue<UIColor>(
                interpolationMode: .identity,
                sourceStops: nil,
                attributeName: "size",
                options: [.defaultValue: MGLStyleValue<UIColor>(rawValue: .green)]
            )
            circleStyleLayer.circleColor = expectedSourceIdentityValue
            assertColorValuesEqual(circleStyleLayer.circleColor, expectedSourceIdentityValue)

        #elseif os(macOS)

            // data-driven, camera function with exponential color stop values
            let redGreenStops = [
                0: MGLStyleValue<NSColor>(rawValue: .red),
                10: MGLStyleValue<NSColor>(rawValue: .red),
                15: MGLStyleValue<NSColor>(rawValue: .green)
            ]
            let expectedCircleColorValue = MGLStyleValue<NSColor>(
                interpolationMode: .exponential,
                cameraStops: redGreenStops,
                options: [.interpolationBase: 10.0]
            )
            circleStyleLayer.circleColor = expectedCircleColorValue
            assertColorValuesEqual(circleStyleLayer.circleColor, expectedCircleColorValue)

            // data-driven, source function with categorical color stop values with string typed keys
            let redOnlyStops = [
                "red": MGLStyleValue<NSColor>(rawValue: .red)
            ]
            let expectedRedCategoricalValue = MGLStyleValue<NSColor>(
                interpolationMode: .categorical,

                sourceStops: redOnlyStops,
                attributeName: "red",
                options: [.defaultValue: MGLStyleValue<NSColor>(rawValue: .cyan)]
            )
            circleStyleLayer.circleColor = expectedRedCategoricalValue
            assertColorValuesEqual(circleStyleLayer.circleColor, expectedRedCategoricalValue)

            // data-driven, source function with categorical color stop values with integer attribute keys
            let greenOrangeStops = [
                0: MGLStyleValue<NSColor>(rawValue: .green),
                100: MGLStyleValue<NSColor>(rawValue: .orange)
            ]
            let expectedGreenOrangeCategoricalValue = MGLStyleValue<NSColor>(
                interpolationMode: .categorical,
                sourceStops: greenOrangeStops,
                attributeName: "temp",
                options: [.defaultValue: MGLStyleValue<NSColor>(rawValue: .red)]
            )
            circleStyleLayer.circleColor = expectedGreenOrangeCategoricalValue
            assertColorValuesEqual(circleStyleLayer.circleColor, expectedGreenOrangeCategoricalValue)

            // data-driven, source function with exponential color stop values
            let expectedRedGreenSourceExponentialValue = MGLStyleValue<NSColor>(
                interpolationMode: .exponential,
                sourceStops: redGreenStops,
                attributeName: "temp",
                options: nil
            )
            circleStyleLayer.circleColor = expectedRedGreenSourceExponentialValue
            assertColorValuesEqual(circleStyleLayer.circleColor, expectedRedGreenSourceExponentialValue)

            // data-driven, identity source function
            let expectedSourceIdentityValue = MGLStyleValue<NSColor>(
                interpolationMode: .identity,
                sourceStops: nil,
                attributeName: "size",
                options: nil
            )
            circleStyleLayer.circleColor = expectedSourceIdentityValue
            assertColorValuesEqual(circleStyleLayer.circleColor, expectedSourceIdentityValue)

        #endif

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
        let radiusCompositeCategoricalStops: [NSNumber: [String: MGLStyleValue<NSNumber>]] = [
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
        let radiusCompositeExponentialOrIntervalStops: [NSNumber: [NSNumber: MGLStyleValue<NSNumber>]] = [
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
