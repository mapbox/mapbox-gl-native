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
            let stops = [
                0: MGLStyleValue<NSColor>(rawValue: .yellow),
                2.5: MGLStyleValue(rawValue: .orange),
                5: MGLStyleValue(rawValue: .red),
                7.5: MGLStyleValue(rawValue: .blue),
                10: MGLStyleValue(rawValue: .white),
            ]
        #else
            let stops = [
                0: MGLStyleValue<UIColor>(rawValue: .yellow),
                2.5: MGLStyleValue(rawValue: .orange),
                5: MGLStyleValue(rawValue: .red),
                7.5: MGLStyleValue(rawValue: .blue),
                10: MGLStyleValue(rawValue: .white),
            ]
        #endif
        //#-end-example-code
        
        let _ = MGLStyleValue(interpolationMode: .exponential, cameraStops: stops, options: nil)
    }
    
    func testUsingStyleFunctionsAtRuntime$Linear() {
        //#-example-code
        let url = URL(string: "https://earthquake.usgs.gov/earthquakes/feed/v1.0/summary/all_week.geojson")!
        let symbolSource = MGLSource(identifier: "source")
        let symbolLayer = MGLSymbolStyleLayer(identifier: "place-city-sm", source: symbolSource)
        
        let source = MGLShapeSource(identifier: "earthquakes", url: url, options: nil)
        mapView.style?.addSource(source)
        
        #if os(macOS)
            let stops = [
                0: MGLStyleValue<NSColor>(rawValue: .yellow),
                2.5: MGLStyleValue(rawValue: .orange),
                5: MGLStyleValue(rawValue: .red),
                7.5: MGLStyleValue(rawValue: .blue),
                10: MGLStyleValue(rawValue: .white),
            ]
        #else
            let stops = [
                0: MGLStyleValue<UIColor>(rawValue: .yellow),
                2.5: MGLStyleValue(rawValue: .orange),
                5: MGLStyleValue(rawValue: .red),
                7.5: MGLStyleValue(rawValue: .blue),
                10: MGLStyleValue(rawValue: .white),
            ]
        #endif
        
        let layer = MGLCircleStyleLayer(identifier: "circles", source: source)
        #if os(macOS)
            layer.circleColor = MGLStyleValue(interpolationMode: .exponential,
                                              sourceStops: stops,
                                              attributeName: "mag",
                                              options: [.defaultValue: MGLStyleValue<NSColor>(rawValue: .green)])
        #else
            layer.circleColor = MGLStyleValue(interpolationMode: .exponential,
                                              sourceStops: stops,
                                              attributeName: "mag",
                                              options: [.defaultValue: MGLStyleValue<UIColor>(rawValue: .green)])
        #endif
        layer.circleRadius = MGLStyleValue(rawValue: 10)
        mapView.style?.insertLayer(layer, below: symbolLayer)
        //#-end-example-code
    }
    
    func testUsingStyleFunctionsAtRuntime$Exponential() {
        let source = MGLShapeSource(identifier: "circles", shape: nil, options: nil)
        let layer = MGLCircleStyleLayer(identifier: "circles", source: source)
        
        //#-example-code
        let stops = [
            12: MGLStyleValue<NSNumber>(rawValue: 0.5),
            14: MGLStyleValue(rawValue: 2),
            18: MGLStyleValue(rawValue: 18),
        ]
        
        layer.circleRadius = MGLStyleValue(interpolationMode: .exponential,
                                           cameraStops: stops,
                                           options: [.interpolationBase: 1.5])
        //#-end-example-code
    }
    
    func testUsingStyleFunctionsAtRuntime$Interval() {
        let source = MGLShapeSource(identifier: "circles", shape: nil, options: nil)
        let layer = MGLCircleStyleLayer(identifier: "circles", source: source)
        
        //#-example-code
        #if os(macOS)
            let stops = [
                0: MGLStyleValue<NSColor>(rawValue: .yellow),
                2.5: MGLStyleValue(rawValue: .orange),
                5: MGLStyleValue(rawValue: .red),
                7.5: MGLStyleValue(rawValue: .blue),
                10: MGLStyleValue(rawValue: .white),
            ]
            
            layer.circleColor = MGLStyleValue(interpolationMode: .interval,
                                              sourceStops: stops,
                                              attributeName: "mag",
                                              options: [.defaultValue: MGLStyleValue<NSColor>(rawValue: .green)])
        #else
            let stops = [
                0: MGLStyleValue<UIColor>(rawValue: .yellow),
                2.5: MGLStyleValue(rawValue: .orange),
                5: MGLStyleValue(rawValue: .red),
                7.5: MGLStyleValue(rawValue: .blue),
                10: MGLStyleValue(rawValue: .white),
            ]
            
            layer.circleColor = MGLStyleValue(interpolationMode: .interval,
                                              sourceStops: stops,
                                              attributeName: "mag",
                                              options: [.defaultValue: MGLStyleValue<UIColor>(rawValue: .green)])
        #endif
        //#-end-example-code
    }
    
    func testUsingStyleFunctionsAtRuntime$Categorical() {
        let source = MGLShapeSource(identifier: "circles", shape: nil, options: nil)
        let layer = MGLCircleStyleLayer(identifier: "circles", source: source)
        
        //#-example-code
        #if os(macOS)
            let categoricalStops = [
                "earthquake": MGLStyleValue<NSColor>(rawValue: .orange),
                "explosion": MGLStyleValue(rawValue: .red),
                "quarry blast": MGLStyleValue(rawValue: .yellow),
            ]
            
            layer.circleColor = MGLStyleValue(interpolationMode: .categorical,
                                              sourceStops: categoricalStops,
                                              attributeName: "type",
                                              options: [.defaultValue: MGLStyleValue<NSColor>(rawValue: .blue)])
        #else
            let categoricalStops = [
                "earthquake": MGLStyleValue<UIColor>(rawValue: .orange),
                "explosion": MGLStyleValue(rawValue: .red),
                "quarry blast": MGLStyleValue(rawValue: .yellow),
            ]
            
            layer.circleColor = MGLStyleValue(interpolationMode: .categorical,
                                              sourceStops: categoricalStops,
                                              attributeName: "type",
                                              options: [.defaultValue: MGLStyleValue<UIColor>(rawValue: .blue)])
        #endif
        //#-end-example-code
    }
    
    func testUsingStyleFunctionsAtRuntime$Identity() {
        let source = MGLShapeSource(identifier: "circles", shape: nil, options: nil)
        let layer = MGLCircleStyleLayer(identifier: "circles", source: source)
        
        //#-example-code
        layer.circleRadius = MGLStyleValue(interpolationMode: .identity,
                                           sourceStops: nil,
                                           attributeName: "mag",
                                           options: [.defaultValue: MGLStyleValue<NSNumber>(rawValue: 0)])
        //#-end-example-code
    }
}
