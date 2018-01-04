import XCTest
import Mapbox
#if os(iOS)
    import UIKit
#else
    import Cocoa
#endif

/**
 Test cases that ensure the inline examples in the project documentation
 compile.

 To add an example:
 1. Add a test case named in the form testMGLClass or testMGLClass$method.
 2. Wrap the code you'd like to appear in the documentation within the
    following comment blocks:
    ```
    //#-example-code
    ...
    //#-end-example-code
    ```
 3. Insert an empty Swift code block inside the header file where you'd like the
    example code to be inserted.
 4. Run `make darwin-update-examples` to extract example code from the test
    method below and insert it into the header.
 */
class MGLDocumentationExampleTests: XCTestCase, MGLMapViewDelegate {
    var mapView: MGLMapView!
    var styleLoadingExpectation: XCTestExpectation!
    static let styleURL = Bundle(for: MGLDocumentationExampleTests.self).url(forResource: "one-liner", withExtension: "json")!

    override func setUp() {
        super.setUp()
        mapView = MGLMapView(frame: CGRect(x: 0, y: 0, width: 256, height: 256), styleURL: MGLDocumentationExampleTests.styleURL)
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
    
    func testMGLShape$shapeWithData_encoding_error_() {
        let mainBundle = Bundle(for: MGLDocumentationExampleTests.self)
        
        //#-example-code
        let url = mainBundle.url(forResource: "amsterdam", withExtension: "geojson")!
        let data = try! Data(contentsOf: url)
        let feature = try! MGLShape(data: data, encoding: String.Encoding.utf8.rawValue) as! MGLShapeCollectionFeature
        //#-end-example-code
        
        XCTAssertNotNil(feature.shapes.first as? MGLPolygonFeature)
    }

    func testMGLShapeSource() {
        //#-example-code
        var coordinates: [CLLocationCoordinate2D] = [
            CLLocationCoordinate2D(latitude: 37.77, longitude: -122.42),
            CLLocationCoordinate2D(latitude: 38.91, longitude: -77.04),
        ]
        let polyline = MGLPolylineFeature(coordinates: &coordinates, count: UInt(coordinates.count))
        let source = MGLShapeSource(identifier: "lines", features: [polyline], options: nil)
        mapView.style?.addSource(source)
        //#-end-example-code

        XCTAssertNotNil(mapView.style?.source(withIdentifier: "lines"))
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
        mapView.style?.addSource(source)
        //#-end-example-code

        XCTAssertNotNil(mapView.style?.source(withIdentifier: "clouds"))
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
        mapView.style?.addSource(source)
        //#-end-example-code

        XCTAssertNotNil(mapView.style?.source(withIdentifier: "pois"))
    }
    
    func testMGLPolyline() {
        //#-example-code
        let coordinates = [
            CLLocationCoordinate2D(latitude: 35.68476, longitude: -220.24257),
            CLLocationCoordinate2D(latitude: 37.78428, longitude: -122.41310)
        ]
        let polyline = MGLPolyline(coordinates: coordinates, count: UInt(coordinates.count))
        //#-end-example-code
        
        XCTAssertNotNil(polyline)
    }

    func testMGLImageSource() {
        //#-example-code
        let coordinates = MGLCoordinateQuad(
          topLeft: CLLocationCoordinate2D(latitude: 46.437, longitude: -80.425),
          bottomLeft: CLLocationCoordinate2D(latitude: 37.936, longitude: -80.425),
          bottomRight: CLLocationCoordinate2D(latitude: 37.936, longitude: -71.516),
          topRight: CLLocationCoordinate2D(latitude: 46.437, longitude: -71.516))
        let source = MGLImageSource(identifier: "radar", coordinateQuad: coordinates, url: URL(string: "https://www.mapbox.com/mapbox-gl-js/assets/radar.gif")!)
        mapView.style?.addSource(source)
        //#-end-example-code

        XCTAssertNotNil(mapView.style?.source(withIdentifier: "radar"))
    }

    func testMGLCircleStyleLayer() {
        let population = MGLVectorSource(identifier: "population", configurationURL: URL(string: "https://example.com/style.json")!)
        mapView.style?.addSource(population)
        
        //#-example-code
        let layer = MGLCircleStyleLayer(identifier: "circles", source: population)
        layer.sourceLayerIdentifier = "population"
        layer.circleColor = MGLStyleValue(rawValue: .green)
        layer.circleRadius = MGLStyleValue(interpolationMode: .exponential,
                                           cameraStops: [12: MGLStyleValue(rawValue: 2),
                                                         22: MGLStyleValue(rawValue: 180)],
                                           options: [.interpolationBase: 1.75])
        layer.circleOpacity = MGLStyleValue(rawValue: 0.7)
        layer.predicate = NSPredicate(format: "%K == %@", "marital-status", "married")
        mapView.style?.addLayer(layer)
        //#-end-example-code

        XCTAssertNotNil(mapView.style?.layer(withIdentifier: "circles"))
    }

    func testMGLLineStyleLayer() {
        let trails = MGLVectorSource(identifier: "trails", configurationURL: URL(string: "https://example.com/style.json")!)
        mapView.style?.addSource(trails)

        //#-example-code
        let layer = MGLLineStyleLayer(identifier: "trails-path", source: trails)
        layer.sourceLayerIdentifier = "trails"
        layer.lineWidth = MGLStyleValue(interpolationMode: .exponential,
                                        cameraStops: [14: MGLStyleValue(rawValue: 2),
                                                      18: MGLStyleValue(rawValue: 20)],
                                        options: [.interpolationBase: 1.5])
        layer.lineColor = MGLStyleValue(rawValue: .brown)
        layer.lineCap = MGLStyleValue(rawValue: NSValue(mglLineCap: .round))
        layer.predicate = NSPredicate(format: "%K == %@", "trail-type", "mountain-biking")
        mapView.style?.addLayer(layer)
        //#-end-example-code

        XCTAssertNotNil(mapView.style?.layer(withIdentifier: "trails-path"))
    }

    func testMGLFillStyleLayer() {
        let parks = MGLVectorSource(identifier: "parks", configurationURL: URL(string: "https://example.com/style.json")!)
        mapView.style?.addSource(parks)

        //#-example-code
        let layer = MGLFillStyleLayer(identifier: "parks", source: parks)
        layer.sourceLayerIdentifier = "parks"
        layer.fillColor = MGLStyleValue(rawValue: .green)
        layer.predicate = NSPredicate(format: "type == %@", "national-park")
        mapView.style?.addLayer(layer)
        //#-end-example-code

        XCTAssertNotNil(mapView.style?.layer(withIdentifier: "parks"))
    }
    
    func testMGLFillExtrusionStyleLayer() {
        let buildings = MGLVectorSource(identifier: "buildings", configurationURL: URL(string: "https://example.com/style.json")!)
        mapView.style?.addSource(buildings)
        
        //#-example-code
        let layer = MGLFillExtrusionStyleLayer(identifier: "buildings", source: buildings)
        layer.sourceLayerIdentifier = "building"
        layer.fillExtrusionHeight = MGLStyleValue(interpolationMode: .identity, sourceStops: nil, attributeName: "height", options: nil)
        layer.fillExtrusionBase = MGLStyleValue(interpolationMode: .identity, sourceStops: nil, attributeName: "min_height", options: nil)
        layer.predicate = NSPredicate(format: "extrude == 'true'")
        mapView.style?.addLayer(layer)
        //#-end-example-code
        
        XCTAssertNotNil(mapView.style?.layer(withIdentifier: "buildings"))
    }

    func testMGLSymbolStyleLayer() {
        let pois = MGLVectorSource(identifier: "pois", configurationURL: URL(string: "https://example.com/style.json")!)
        mapView.style?.addSource(pois)

        //#-example-code
        let layer = MGLSymbolStyleLayer(identifier: "coffeeshops", source: pois)
        layer.sourceLayerIdentifier = "pois"
        layer.iconImageName = MGLStyleValue(rawValue: "coffee")
        layer.iconScale = MGLStyleValue(rawValue: 0.5)
        layer.text = MGLStyleValue(rawValue: "{name}")
        #if os(macOS)
            var vector = CGVector(dx: 10, dy: 0)
            layer.textTranslation = MGLStyleValue(rawValue: NSValue(bytes: &vector, objCType: "{CGVector=dd}"))
        #else
            layer.textTranslation = MGLStyleValue(rawValue: NSValue(cgVector: CGVector(dx: 10, dy: 0)))
        #endif
        layer.textJustification = MGLStyleValue(rawValue: NSValue(mglTextJustification: .left))
        layer.textAnchor = MGLStyleValue(rawValue: NSValue(mglTextAnchor: .left))
        layer.predicate = NSPredicate(format: "%K == %@", "venue-type", "coffee")
        mapView.style?.addLayer(layer)
        //#-end-example-code

        XCTAssertNotNil(mapView.style?.layer(withIdentifier: "coffeeshops"))
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
        mapView.style?.addSource(source)

        //#-example-code
        let layer = MGLRasterStyleLayer(identifier: "clouds", source: source)
        layer.rasterOpacity = MGLStyleValue(rawValue: 0.5)
        mapView.style?.addLayer(layer)
        //#-end-example-code

        XCTAssertNotNil(mapView.style?.layer(withIdentifier: "clouds"))
    }

    func testMGLVectorStyleLayer$predicate() {
        let terrain = MGLVectorSource(identifier: "terrain", configurationURL: URL(string: "https://example.com/style.json")!)
        mapView.style?.addSource(terrain)

        //#-example-code
        let layer = MGLLineStyleLayer(identifier: "contour", source: terrain)
        layer.sourceLayerIdentifier = "contours"
        layer.predicate = NSPredicate(format: "(index == 5 || index == 10) && ele >= 1500.0")
        mapView.style?.addLayer(layer)
        //#-end-example-code

        XCTAssertNotNil(mapView.style?.layer(withIdentifier: "contour"))
    }
    
    func testMGLMapView() {
        //#-example-code
        #if os(macOS)
            class MapClickGestureRecognizer: NSClickGestureRecognizer {
                override func shouldRequireFailure(of otherGestureRecognizer: NSGestureRecognizer) -> Bool {
                    return otherGestureRecognizer is NSClickGestureRecognizer
                }
            }
        #else
            let mapTapGestureRecognizer = UITapGestureRecognizer(target: self, action: #selector(myCustomFunction))
            for recognizer in mapView.gestureRecognizers! where recognizer is UITapGestureRecognizer {
                mapTapGestureRecognizer.require(toFail: recognizer)
            }
            mapView.addGestureRecognizer(mapTapGestureRecognizer)
        #endif
        //#-end-example-code
    }
    
    func testMGLMapSnapshotter() {
        let expectation = self.expectation(description: "MGLMapSnapshotter should produce a snapshot")
        #if os(macOS)
            var image: NSImage? {
                didSet {
                    expectation.fulfill()
                }
            }
        #else
            var image: UIImage? {
                didSet {
                    expectation.fulfill()
                }
            }
        #endif
        
        class MGLStyle {
            static func satelliteStreetsStyleURL() -> URL {
                return MGLDocumentationExampleTests.styleURL
            }
        }
        
        //#-example-code
        let camera = MGLMapCamera(lookingAtCenter: CLLocationCoordinate2D(latitude: 37.7184, longitude: -122.4365), fromDistance: 100, pitch: 20, heading: 0)

        let options = MGLMapSnapshotOptions(styleURL: MGLStyle.satelliteStreetsStyleURL(), camera: camera, size: CGSize(width: 320, height: 480))
        options.zoomLevel = 10

        let snapshotter = MGLMapSnapshotter(options: options)
        snapshotter.start { (snapshot, error) in
            if let error = error {
                fatalError(error.localizedDescription)
            }
            
            image = snapshot?.image
        }
        //#-end-example-code
        
        wait(for: [expectation], timeout: 1)
    }
    
    // For testMGLMapView().
    func myCustomFunction() {}
}
