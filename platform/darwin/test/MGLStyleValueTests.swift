import XCTest
import Mapbox


extension MGLStyleValueTests {
    
    func testConstantValues() {
        let shapeSource = MGLShapeSource(identifier: "test", shape: nil, options: nil)
        let symbolStyleLayer = MGLSymbolStyleLayer(identifier: "test", source: shapeSource)
        
        // Boolean
        symbolStyleLayer.iconAllowsOverlap = MGLStyleConstantValue(rawValue: true)
        XCTAssertEqual((symbolStyleLayer.iconAllowsOverlap as! MGLStyleConstantValue<NSNumber>).rawValue, true)
        
        // Number
        symbolStyleLayer.iconHaloWidth = MGLStyleConstantValue(rawValue: 3)
        XCTAssertEqual((symbolStyleLayer.iconHaloWidth as! MGLStyleConstantValue<NSNumber>).rawValue, 3)
        
        // String
        symbolStyleLayer.text = MGLStyleConstantValue(rawValue: "{name}")
        XCTAssertEqual((symbolStyleLayer.text as! MGLStyleConstantValue<NSString>).rawValue, "{name}")
    }
    
    func testFunctions() {
        let shapeSource = MGLShapeSource(identifier: "test", shape: nil, options: nil)
        let symbolStyleLayer = MGLSymbolStyleLayer(identifier: "test", source: shapeSource)
        
        // Boolean
        let stops: [NSNumber: MGLStyleValue<NSNumber>] = [
            1: MGLStyleValue(rawValue: true),
            2: MGLStyleValue(rawValue: false),
            3: MGLStyleValue(rawValue: true),
            4: MGLStyleValue(rawValue: false),
            ]
        symbolStyleLayer.iconAllowsOverlap = MGLStyleFunction<NSNumber>(interpolationBase: 1, stops: stops)
        XCTAssertEqual((symbolStyleLayer.iconAllowsOverlap as! MGLStyleFunction<NSNumber>), MGLStyleFunction(interpolationBase: 1, stops: stops))
    }
}
