import XCTest
import Mapbox

/**
 Test cases that ensure the inline examples in the jazzy guides compile.

 To add an example:
 1. Add a test case named in the form `testGuideName$ExampleName`.
 2. Wrap the code you’d like to appear in the documentation within the
    following comment blocks:
    ```
    //#-example-code
    ...
    //#-end-example-code
    ```
 3. Insert a call to `guideExample()`  where you’d like the example code to be
    inserted in the guide’s Markdown.
    ```
    <%- guideExample('GuideName', 'ExampleName', 'iOS') %>
    ```
 4. Run `make darwin-style-code` to extract example code from the test method
    below and insert it into the guide.
 */
class MGLDocumentationGuideTests: XCTestCase, MGLMapViewDelegate {
    var mapView: MGLMapView!
    var styleLoadingExpectation: XCTestExpectation!
    
    override func setUp() {
        super.setUp()
        let styleURL = Bundle(for: MGLDocumentationGuideTests.self).url(forResource: "one-liner", withExtension: "json")
        mapView = MGLMapView(frame: CGRect(x: 0, y: 0, width: 256, height: 256), styleURL: styleURL)
        mapView.delegate = self
        styleLoadingExpectation = expectation(description: "Map view should finish loading style")
        waitForExpectations(timeout: 10, handler: nil)
    }
    
    override func tearDown() {
        mapView = nil
        styleLoadingExpectation = nil
        super.tearDown()
    }
    
    func mapView(_ mapView: MGLMapView, didFinishLoading style: MGLStyle) {
        styleLoadingExpectation.fulfill()
    }
    
    func testMigratingToExpressions$Stops() {
        //#-example-code
        #if os(macOS)
            let stops: [NSNumber: NSColor] = [
                0: .yellow,
                2.5: .orange,
                5: .red,
                7.5: .blue,
                10: .white,
            ]
        #else
            let stops: [NSNumber: UIColor] = [
                0: .yellow,
                2.5: .orange,
                5: .red,
                7.5: .blue,
                10: .white,
            ]
        #endif
        //#-end-example-code
        
        let _ = NSExpression(format: "mgl_step:from:stops:(mag, %@, %@)",
                             stops[0]!, stops)
    }
    
    func testMigratingToExpressions$Linear() {
        //#-example-code
        let url = URL(string: "https://earthquake.usgs.gov/earthquakes/feed/v1.0/summary/all_week.geojson")!
        let symbolSource = MGLSource(identifier: "source")
        let symbolLayer = MGLSymbolStyleLayer(identifier: "place-city-sm", source: symbolSource)
        
        let source = MGLShapeSource(identifier: "earthquakes", url: url, options: nil)
        mapView.style?.addSource(source)
        
        #if os(macOS)
            let stops: [NSNumber: NSColor] = [
                0: .yellow,
                2.5: .orange,
                5: .red,
                7.5: .blue,
                10: .white,
            ]
        #else
            let stops: [NSNumber: UIColor] = [
                0: .yellow,
                2.5: .orange,
                5: .red,
                7.5: .blue,
                10: .white,
            ]
        #endif
        
        let layer = MGLCircleStyleLayer(identifier: "circles", source: source)
        #if os(macOS)
            layer.circleColor = NSExpression(format: "mgl_interpolate:withCurveType:parameters:stops:(mag, 'linear', nil, %@)",
                                             stops)
        #else
            layer.circleColor = NSExpression(format: "mgl_interpolate:withCurveType:parameters:stops:(mag, 'linear', nil, %@)",
                                             stops)
        #endif
        layer.circleRadius = NSExpression(forConstantValue: 10)
        mapView.style?.insertLayer(layer, below: symbolLayer)
        //#-end-example-code
    }
    
    func testMigratingToExpressions$LinearConvenience() {
        let source = MGLShapeSource(identifier: "circles", shape: nil, options: nil)
        let layer = MGLCircleStyleLayer(identifier: "circles", source: source)
        
        #if os(macOS)
        let stops: [NSNumber: NSColor] = [
            0: .yellow,
            2.5: .orange,
            5: .red,
            7.5: .blue,
            10: .white,
            ]
        #else
        let stops: [NSNumber: UIColor] = [
            0: .yellow,
            2.5: .orange,
            5: .red,
            7.5: .blue,
            10: .white,
            ]
        #endif
        
        //#-example-code
        layer.circleColor = NSExpression(forMGLInterpolating: NSExpression(forKeyPath: "mag"), curveType: .linear, parameters: nil, stops: NSExpression(forConstantValue: stops))
        //#-end-example-code
        
        layer.circleRadius = NSExpression(forConstantValue: 10)
        mapView.style?.addLayer(layer)
        
    }
    func testMigratingToExpressions$Exponential() {
        let source = MGLShapeSource(identifier: "circles", shape: nil, options: nil)
        let layer = MGLCircleStyleLayer(identifier: "circles", source: source)
        
        //#-example-code
        let stops = [
            12: 0.5,
            14: 2,
            18: 18,
        ]
        
        layer.circleRadius = NSExpression(format: "mgl_interpolate:withCurveType:parameters:stops:($zoomLevel, 'exponential', 1.5, %@)",
                                          stops)
        //#-end-example-code
    }
    
    func testMigratingToExpressions$ExponentialConvenience() {
        let source = MGLShapeSource(identifier: "circles", shape: nil, options: nil)
        let layer = MGLCircleStyleLayer(identifier: "circles", source: source)
        
        //#-example-code
        let stops = [
            12: 0.5,
            14: 2,
            18: 18,
            ]
        
        layer.circleRadius =  NSExpression(forMGLInterpolating: NSExpression.zoomLevelVariable, curveType: MGLExpressionInterpolationMode.exponential, parameters: NSExpression(forConstantValue: 1.5), stops: NSExpression(forConstantValue: stops))
        //#-end-example-code
    }
    func testMigratingToExpressions$Interval() {
        let source = MGLShapeSource(identifier: "circles", shape: nil, options: nil)
        let layer = MGLCircleStyleLayer(identifier: "circles", source: source)
        
        //#-example-code
        #if os(macOS)
            let stops: [NSNumber: NSColor] = [
                0: .yellow,
                2.5: .orange,
                5: .red,
                7.5: .blue,
                10: .white,
            ]
            
            layer.circleColor = NSExpression(format: "mgl_step:from:stops:(mag, %@, %@)",
                                             NSColor.green, stops)
        #else
            let stops: [NSNumber: UIColor] = [
                0: .yellow,
                2.5: .orange,
                5: .red,
                7.5: .blue,
                10: .white,
            ]
            
            layer.circleColor = NSExpression(format: "mgl_step:from:stops:(mag, %@, %@)",
                                             UIColor.green, stops)
        #endif
        //#-end-example-code
    }
    
    func testMigratingToExpressions$Categorical() {
        let source = MGLShapeSource(identifier: "circles", shape: nil, options: nil)
        let layer = MGLCircleStyleLayer(identifier: "circles", source: source)
        
        //#-example-code
        #if os(macOS)
            let defaultColor = NSColor.blue
            layer.circleColor = NSExpression(
            format: "MGL_MATCH(type, 'earthquake', %@, 'explosion', %@, 'quarry blast', %@, %@)",
                NSColor.orange, NSColor.red, NSColor.yellow, defaultColor)
        #else
            let defaultColor = UIColor.blue
            layer.circleColor = NSExpression(format: "MGL_MATCH(type, 'earthquake', %@, 'explosion', %@, 'quarry blast', %@, %@)",
                UIColor.orange, UIColor.red, UIColor.yellow, defaultColor)
        #endif
        //#-end-example-code
    }
    
    func testMigratingToExpressions$CategoricalValue() {
        let source = MGLShapeSource(identifier: "circles", shape: nil, options: nil)
        let layer = MGLCircleStyleLayer(identifier: "circles", source: source)
        
        //#-example-code
        #if os(macOS)
        let stops : [String : NSColor] = ["earthquake" : NSColor.orange,
                                          "explosion" : NSColor.red,
                                          "quarry blast" : NSColor.yellow]
        layer.circleColor = NSExpression(
            format: "FUNCTION(%@, 'valueForKeyPath:', type)",
            stops)
        #else
        let stops : [String : UIColor] = ["earthquake" : UIColor.orange,
                                          "explosion" : UIColor.red,
                                          "quarry blast" : UIColor.yellow]
        layer.circleColor = NSExpression(
            format: "FUNCTION(%@, 'valueForKeyPath:', type)",
            stops)
        #endif
        //#-end-example-code
    }
    func testMigratingToExpressions$Identity() {
        let source = MGLShapeSource(identifier: "circles", shape: nil, options: nil)
        let layer = MGLCircleStyleLayer(identifier: "circles", source: source)
        
        //#-example-code
        layer.circleRadius = NSExpression(forKeyPath: "mag")
        //#-end-example-code
    }
    
    func testMigratingToExpressions$Multiply() {
        let source = MGLShapeSource(identifier: "circles", shape: nil, options: nil)
        let layer = MGLCircleStyleLayer(identifier: "circles", source: source)
        
        //#-example-code
        layer.circleRadius = NSExpression(forFunction: "multiply:by:", arguments: [NSExpression(forKeyPath: "mag"), 3])
        //#-end-example-code
    }
    
    func testMigratingToExpressions$Cast() {
        let source = MGLShapeSource(identifier: "circles", shape: nil, options: nil)
        
        //#-example-code
        let magnitudeLayer = MGLSymbolStyleLayer(identifier: "mag-layer", source: source)
        magnitudeLayer.text = NSExpression(format: "CAST(mag, 'NSString')")
        mapView.style?.addLayer(magnitudeLayer)
        //#-end-example-code
    }
}
