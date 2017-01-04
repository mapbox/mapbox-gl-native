import XCTest
import Mapbox
import UIKit

/**
 Test cases that ensure the inline examples in the project documentation
 compile.

 There is a run script build phase for the dynamic & static iOS targets
 that invokes `/platform/ios/scrips/add-examples-to-docs.js`. This script
 will pull example code out of this test file and replace the corresponding
 placeholder comment in the built header files.

 Adding examples:
 1. Add a test case named in the form testMGLClass or testMGLClass$method.
 2. Wrap the code you'd like to appear in the documentation within the
    following comment blocks:
    ```
    //#-example-code
    ...
    //#-end-example-code
    ```
 3. Insert a comment `<!--EXAMPLE: ExampleToken-->` inside the header file
    where you'd like the example code to be inserted.
 */
class MGLDocumentationExampleTests: XCTestCase {
    var mapView: MGLMapView!

    override func setUp() {
        super.setUp()
        self.mapView = MGLMapView(frame: CGRect(x: 0, y: 0, width: 256, height: 256))
    }

    override func tearDown() {
        self.mapView = nil
        super.tearDown()
    }

    func testMGLShapeSource() {
        //#-example-code
        var coordinates: [CLLocationCoordinate2D] = [
            CLLocationCoordinate2D(latitude: 37.77, longitude: -122.42),
            CLLocationCoordinate2D(latitude: 38.91, longitude: -77.04),
        ]
        let polyline = MGLPolylineFeature(coordinates: &coordinates, count: UInt(coordinates.count))
        let shape = MGLShapeCollectionFeature(shapes: [polyline])
        let source = MGLShapeSource(identifier: "lines", shape: shape, options: nil)
        mapView.style.addSource(source)
        //#-end-example-code

        XCTAssertNotNil(mapView.style.source(withIdentifier: "lines"))
    }

    func testMGLRasterSource() {
        //#-example-code
        let source = MGLRasterSource(identifier: "clouds", tileURLTemplates: ["https://example.com/raster-tiles/{z}/{x}/{y}.png"], options: [
            .minimumZoomLevel: 9,
            .maximumZoomLevel: 16,
            .tileSize: 512,
            .attributionInfos: [
                MGLAttributionInfo(title: NSAttributedString(string: "© Mapbox"), url: URL(string: "http://mapbox.com"))
            ]
        ])
        mapView.style.addSource(source)
        //#-end-example-code

        XCTAssertNotNil(mapView.style.source(withIdentifier: "clouds"))
    }

    func testMGLVectorSource() {
        //#-example-code
        let source = MGLVectorSource(identifier: "pois", tileURLTemplates: ["https://example.com/vector-tiles/{z}/{x}/{y}.mvt"], options: [
            .minimumZoomLevel: 9,
            .maximumZoomLevel: 16,
            .attributionInfos: [
                MGLAttributionInfo(title: NSAttributedString(string: "© Mapbox"), url: URL(string: "http://mapbox.com"))
            ]
        ])
        mapView.style.addSource(source)
        //#-end-example-code

        XCTAssertNotNil(mapView.style.source(withIdentifier: "pois"))
    }

    func testMGLCircleStyleLayer() {
        let population = MGLVectorSource(identifier: "population", configurationURL: URL(string: "https://example.com/style.json")!)
        mapView.style.addSource(population)
        
        //#-example-code
        let layer = MGLCircleStyleLayer(identifier: "circles", source: population)
        layer.sourceLayerIdentifier = "population"
        layer.circleColor = MGLStyleValue(rawValue: .green)
        layer.circleRadius = MGLStyleValue(interpolationBase: 1.75, stops: [
            12: MGLStyleValue(rawValue: 2),
            22: MGLStyleValue(rawValue: 180)
        ])
        layer.circleOpacity = MGLStyleValue(rawValue: 0.7)
        layer.predicate = NSPredicate(format: "%K == %@", "marital-status", "married")
        mapView.style.addLayer(layer)
        //#-end-example-code

        XCTAssertNotNil(mapView.style.layer(withIdentifier: "circles"))
    }

    func testMGLLineStyleLayer() {
        let trails = MGLVectorSource(identifier: "trails", configurationURL: URL(string: "https://example.com/style.json")!)
        mapView.style.addSource(trails)

        //#-example-code
        let layer = MGLLineStyleLayer(identifier: "trails-path", source: trails)
        layer.sourceLayerIdentifier = "trails"
        layer.lineWidth = MGLStyleValue(interpolationBase: 1.5, stops: [
            14: MGLStyleValue(rawValue: 2),
            18: MGLStyleValue(rawValue: 20),
        ])
        layer.lineColor = MGLStyleValue(rawValue: .brown)
        layer.lineCap = MGLStyleValue(rawValue: NSValue(mglLineCap: .round))
        layer.predicate = NSPredicate(format: "%K == %@", "trail-type", "mountain-biking")
        mapView.style.addLayer(layer)
        //#-end-example-code

        XCTAssertNotNil(mapView.style.layer(withIdentifier: "trails-path"))
    }

    func testMGLFillStyleLayer() {
        let parks = MGLVectorSource(identifier: "parks", configurationURL: URL(string: "https://example.com/style.json")!)
        mapView.style.addSource(parks)

        //#-example-code
        let layer = MGLFillStyleLayer(identifier: "parks", source: parks)
        layer.sourceLayerIdentifier = "parks"
        layer.fillColor = MGLStyleValue(rawValue: .green)
        layer.predicate = NSPredicate(format: "type == %@", "national-park")
        mapView.style.addLayer(layer)
        //#-end-example-code

        XCTAssertNotNil(mapView.style.layer(withIdentifier: "parks"))
    }

    func testMGLSymbolStyleLayer() {
        let pois = MGLVectorSource(identifier: "pois", configurationURL: URL(string: "https://example.com/style.json")!)
        mapView.style.addSource(pois)

        //#-example-code
        let layer = MGLSymbolStyleLayer(identifier: "coffeeshops", source: pois)
        layer.sourceLayerIdentifier = "pois"
        layer.iconImageName = MGLStyleValue(rawValue: "coffee")
        layer.iconScale = MGLStyleValue(rawValue: 0.5)
        layer.textField = MGLStyleValue(rawValue: "{name}")
        layer.textTranslate = MGLStyleValue(rawValue: NSValue(cgVector: CGVector(dx: 10, dy: 0)))
        layer.textJustification = MGLStyleValue(rawValue: NSValue(mglTextJustification: .left))
        layer.textAnchor = MGLStyleValue(rawValue: NSValue(mglTextAnchor: .left))
        layer.predicate = NSPredicate(format: "%K == %@", "venue-type", "coffee")
        mapView.style.addLayer(layer)
        //#-end-example-code

        XCTAssertNotNil(mapView.style.layer(withIdentifier: "coffeeshops"))
    }

    func testMGLRasterStyleLayer() {
        let source = MGLRasterSource(identifier: "clouds", tileURLTemplates: ["https://example.com/raster-tiles/{z}/{x}/{y}.png"], options: [
            .minimumZoomLevel: 9,
            .maximumZoomLevel: 16,
            .tileSize: 512,
            .attributionInfos: [
                MGLAttributionInfo(title: NSAttributedString(string: "© Mapbox"), url: URL(string: "http://mapbox.com"))
            ]
        ])
        mapView.style.addSource(source)

        //#-example-code
        let layer = MGLRasterStyleLayer(identifier: "clouds", source: source)
        layer.rasterOpacity = MGLStyleValue(rawValue: 0.5)
        mapView.style.addLayer(layer)
        //#-end-example-code

        XCTAssertNotNil(mapView.style.layer(withIdentifier: "clouds"))
    }

    /**
     To create a filter with the logic `(index == 10 || index == 5) && ele >= 200`,
     you could create a predicate using `NSCompoundPredicate` along these lines:
     */
    func testMGLVectorStyleLayer$predicate() {
        let terrain = MGLVectorSource(identifier: "terrain", configurationURL: URL(string: "https://example.com/style.json")!)
        mapView.style.addSource(terrain)

        //#-example-code
        let layer = MGLLineStyleLayer(identifier: "contour", source: terrain)
        layer.sourceLayerIdentifier = "contours"
        layer.predicate = NSPredicate(format: "(index == 10 || index == 5) && ele >= 1500.0")
        mapView.style.addLayer(layer)
        //#-end-example-code

        XCTAssertNotNil(mapView.style.layer(withIdentifier: "contour"))
    }
}
