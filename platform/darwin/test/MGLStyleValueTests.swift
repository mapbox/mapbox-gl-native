import XCTest
import Mapbox

class MGLStyleValueTests: XCTestCase {
    func testConstantValues() {
        let geoJSONSource = MGLGeoJSONSource(identifier: "test", features: [], options: nil)
        let symbolStyleLayer = MGLSymbolStyleLayer(identifier: "test", source: geoJSONSource)
        
        // Boolean
        symbolStyleLayer.iconAllowOverlap = MGLStyleConstantValue(rawValue: true)
        XCTAssertEqual((symbolStyleLayer.iconAllowOverlap as! MGLStyleConstantValue<NSNumber>).rawValue, true)
        
        // Number
        symbolStyleLayer.iconHaloWidth = MGLStyleConstantValue(rawValue: 3)
        XCTAssertEqual((symbolStyleLayer.iconHaloWidth as! MGLStyleConstantValue<NSNumber>).rawValue, 3)
        
        // String
        symbolStyleLayer.textField = MGLStyleConstantValue(rawValue: "{name}")
        XCTAssertEqual((symbolStyleLayer.textField as! MGLStyleConstantValue<NSString>).rawValue, "{name}")
    }
    
    func testFunctions() {
        let geoJSONSource = MGLGeoJSONSource(identifier: "test", features: [], options: nil)
        let symbolStyleLayer = MGLSymbolStyleLayer(identifier: "test", source: geoJSONSource)
        
        // Boolean
        let stops: [NSNumber: MGLStyleValue<NSNumber>] = [
            1: MGLStyleValue(rawValue: true),
            2: MGLStyleValue(rawValue: false),
            3: MGLStyleValue(rawValue: true),
            4: MGLStyleValue(rawValue: false),
        ]
        symbolStyleLayer.iconAllowOverlap = MGLStyleFunction<NSNumber>(base: 1, stops: stops)
        XCTAssertEqual((symbolStyleLayer.iconAllowOverlap as! MGLStyleFunction<NSNumber>), MGLStyleFunction(base: 1, stops: stops))
    }
}
