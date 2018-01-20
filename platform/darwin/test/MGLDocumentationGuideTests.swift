import XCTest
import Foundation
import Mapbox
#if os(iOS)
    import UIKit
#else
    import Cocoa
#endif

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
        waitForExpectations(timeout: 1, handler: nil)
    }
    
    override func tearDown() {
        mapView = nil
        styleLoadingExpectation = nil
        super.tearDown()
    }
    
    func mapView(_ mapView: MGLMapView, didFinishLoading style: MGLStyle) {
        styleLoadingExpectation.fulfill()
    }
    
    func testUsingStyleFunctionsAtRuntime$Stops() {
        //#-example-code
        #if os(macOS)
            let stops: [Float: NSColor] = [
                0: .yellow,
                2.5: .orange,
                5: .red,
                7.5: .blue,
                10: .white,
            ]
        #else
            let stops: [Float: UIColor] = [
                0: .yellow,
                2.5: .orange,
                5: .red,
                7.5: .blue,
                10: .white,
            ]
        #endif
        //#-end-example-code
        
        let _ = NSExpression(format: "FUNCTION(mag, 'mgl_stepWithMinimum:stops:', %@, %@)",
                             stops[0]!, stops)
    }
    
    func testUsingStyleFunctionsAtRuntime$Linear() {
        //#-example-code
        let url = URL(string: "https://earthquake.usgs.gov/earthquakes/feed/v1.0/summary/all_week.geojson")!
        let symbolSource = MGLSource(identifier: "source")
        let symbolLayer = MGLSymbolStyleLayer(identifier: "place-city-sm", source: symbolSource)
        
        let source = MGLShapeSource(identifier: "earthquakes", url: url, options: nil)
        mapView.style?.addSource(source)
        
        #if os(macOS)
            let stops: [Float: NSColor] = [
                0: .yellow,
                2.5: .orange,
                5: .red,
                7.5: .blue,
                10: .white,
            ]
        #else
            let stops: [Float: UIColor] = [
                0: .yellow,
                2.5: .orange,
                5: .red,
                7.5: .blue,
                10: .white,
            ]
        #endif
        
        let layer = MGLCircleStyleLayer(identifier: "circles", source: source)
        #if os(macOS)
            layer.circleColor = NSExpression(format: "FUNCTION(mag, 'mgl_interpolateWithCurveType:parameters:stops:', 'linear', nil, %@)",
                                             stops)
        #else
            layer.circleColor = NSExpression(format: "FUNCTION(mag, 'mgl_interpolateWithCurveType:parameters:stops:', 'linear', nil, %@)",
                                             stops)
        #endif
        layer.circleRadius = NSExpression(forConstantValue: 10)
        mapView.style?.insertLayer(layer, below: symbolLayer)
        //#-end-example-code
    }
    
    func testUsingStyleFunctionsAtRuntime$Exponential() {
        let source = MGLShapeSource(identifier: "circles", shape: nil, options: nil)
        let layer = MGLCircleStyleLayer(identifier: "circles", source: source)
        
        //#-example-code
        let stops = [
            12: 0.5,
            14: 2,
            18: 18,
        ]
        
        layer.circleRadius = NSExpression(format: "FUNCTION($zoomLevel, 'mgl_interpolateWithCurveType:parameters:stops:', 'exponential', 1.5, %@)",
                                          stops)
        //#-end-example-code
    }
    
    func testUsingStyleFunctionsAtRuntime$Interval() {
        let source = MGLShapeSource(identifier: "circles", shape: nil, options: nil)
        let layer = MGLCircleStyleLayer(identifier: "circles", source: source)
        
        //#-example-code
        #if os(macOS)
            let stops: [Float: NSColor] = [
                0: .yellow,
                2.5: .orange,
                5: .red,
                7.5: .blue,
                10: .white,
            ]
            
            layer.circleColor = NSExpression(format: "FUNCTION(mag, 'mgl_stepWithMinimum:stops:', %@, %@)",
                                             NSColor.green, stops)
        #else
            let stops: [Float: UIColor] = [
                0: .yellow,
                2.5: .orange,
                5: .red,
                7.5: .blue,
                10: .white,
            ]
            
            layer.circleColor = NSExpression(format: "FUNCTION(mag, 'mgl_stepWithMinimum:stops:', %@, %@)",
                                             UIColor.green, stops)
        #endif
        //#-end-example-code
    }
    
    func testUsingStyleFunctionsAtRuntime$Categorical() {
        let source = MGLShapeSource(identifier: "circles", shape: nil, options: nil)
        let layer = MGLCircleStyleLayer(identifier: "circles", source: source)
        
        //#-example-code
        #if os(macOS)
            let colors: [String: NSColor] = [
                "earthquake": .orange,
                "explosion": .red,
                "quarry blast": .yellow,
            ]
            let defaultColor = NSColor.blue
        #else
            let colors: [String: UIColor] = [
                "earthquake": .orange,
                "explosion": .red,
                "quarry blast": .yellow,
            ]
            let defaultColor = UIColor.blue
        #endif
        
        layer.circleColor = NSExpression(
            format: "TERNARY(FUNCTION(%@, 'valueForKeyPath:', type) != nil, FUNCTION(%@, 'valueForKeyPath:', type), %@)",
            colors, colors, defaultColor)
        //#-end-example-code
    }
    
    func testUsingStyleFunctionsAtRuntime$Identity() {
        let source = MGLShapeSource(identifier: "circles", shape: nil, options: nil)
        let layer = MGLCircleStyleLayer(identifier: "circles", source: source)
        
        //#-example-code
        layer.circleRadius = NSExpression(forKeyPath: "mag")
        //#-end-example-code
    }
}
