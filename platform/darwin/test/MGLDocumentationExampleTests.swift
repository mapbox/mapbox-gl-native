import XCTest
import Mapbox

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

    // Mock MGLOfflineStorage singleton so that it doesn't start long-running tasks that could interfere with other tests.
    fileprivate class MGLOfflineStorageMock {
        static let shared = MGLOfflineStorageMock()
        func addPack(for: MGLOfflineRegion, withContext: Data, completionHandler: MGLOfflinePackAdditionCompletionHandler? = nil) {
            XCTAssert(MGLOfflineStorage.shared.responds(to: #selector(MGLOfflineStorage.shared.addPack(for:withContext:completionHandler:))))
            if let completionHandler = completionHandler {
                completionHandler(nil, NSError(domain: "MGLDocumentationExampleError", code: 0, userInfo: [NSLocalizedDescriptionKey: "\(#function) is mocked and not functional."]))
            }
        }
    }

    override func setUp() {
        super.setUp()
        mapView = MGLMapView(frame: CGRect(x: 0, y: 0, width: 256, height: 256), styleURL: MGLDocumentationExampleTests.styleURL)
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
    
    func testMGLLight() {
        //#-example-code
        let light = MGLLight()
        let position = MGLSphericalPosition(radial: 5, azimuthal: 180, polar: 80)
        light.position = NSExpression(forConstantValue: NSValue(mglSphericalPosition: position))
        light.anchor = NSExpression(forConstantValue: "map")
        mapView.style?.light = light
        //#-end-example-code
        
        XCTAssertNotNil(mapView.style?.light)
    }
    
    func testMGLTilePyramidOfflineRegion() {
        class MGLStyle {
            static var lightStyleURL: URL {
                return MGLDocumentationExampleTests.styleURL
            }
        }

        typealias MGLOfflineStorage = MGLOfflineStorageMock

        //#-example-code
        let northeast = CLLocationCoordinate2D(latitude: 40.989329, longitude: -102.062592)
        let southwest = CLLocationCoordinate2D(latitude: 36.986207, longitude: -109.049896)
        let bbox = MGLCoordinateBounds(sw: southwest, ne: northeast)

        let region = MGLTilePyramidOfflineRegion(styleURL: MGLStyle.lightStyleURL, bounds: bbox, fromZoomLevel: 11, toZoomLevel: 14)
        let context = "Tile Pyramid Region".data(using: .utf8)
        MGLOfflineStorage.shared.addPack(for: region, withContext: context!)
        //#-end-example-code

        XCTAssertNotNil(region)
    }
    
    func testMGLShapeOfflineRegion() {
        class MGLStyle {
            static var lightStyleURL: URL {
                return MGLDocumentationExampleTests.styleURL
            }
        }

        typealias MGLOfflineStorage = MGLOfflineStorageMock
        
        //#-example-code
        var coordinates = [
            CLLocationCoordinate2D(latitude: 45.522585, longitude: -122.685699),
            CLLocationCoordinate2D(latitude: 45.534611, longitude: -122.708873),
            CLLocationCoordinate2D(latitude: 45.530883, longitude: -122.678833)
        ]
        
        let triangle = MGLPolygon(coordinates: &coordinates, count: UInt(coordinates.count))
        let region = MGLShapeOfflineRegion(styleURL: MGLStyle.lightStyleURL, shape: triangle, fromZoomLevel: 11, toZoomLevel: 14)
        let context = "Triangle Region".data(using: .utf8)
        MGLOfflineStorage.shared.addPack(for: region, withContext: context!)
        //#-end-example-code
        
        XCTAssertNotNil(region)
    }
    
    func testMGLOfflinePack() {
        typealias MGLOfflineStorage = MGLOfflineStorageMock

        let northeast = CLLocationCoordinate2D(latitude: 40.989329, longitude: -102.062592)
        let southwest = CLLocationCoordinate2D(latitude: 36.986207, longitude: -109.049896)
        let bbox = MGLCoordinateBounds(sw: southwest, ne: northeast)
        let region = MGLTilePyramidOfflineRegion(styleURL: MGLDocumentationExampleTests.styleURL, bounds: bbox, fromZoomLevel: 11, toZoomLevel: 14)
        let context = "Tile Pyramid Region".data(using: .utf8)!

        //#-example-code
        MGLOfflineStorage.shared.addPack(for: region, withContext: context) { (pack, error) in
            guard let pack = pack else {
                // If adding the pack fails, log an error to console.
                print("Error:", error?.localizedDescription ?? "unknown error adding pack at \(#file)(\(#line)) in \(#function)")
                return
            }

            // Start an MGLOfflinePack download
            pack.resume()
        }
        //#-end-example-code
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

    func testMGLRasterTileSource() {
        //#-example-code
        let source = MGLRasterTileSource(identifier: "clouds", tileURLTemplates: ["https://example.com/raster-tiles/{z}/{x}/{y}.png"], options: [
            .minimumZoomLevel: 9,
            .maximumZoomLevel: 16,
            .tileSize: 512,
            .attributionInfos: [
                MGLAttributionInfo(title: NSAttributedString(string: "© Mapbox"), url: URL(string: "https://mapbox.com"))
            ]
        ])
        mapView.style?.addSource(source)
        //#-end-example-code

        XCTAssertNotNil(mapView.style?.source(withIdentifier: "clouds"))
    }
    
    func testMGLRasterDEMSource() {
        // We want to use mapbox.terrain-rgb in the example, but using a mapbox:
        // URL requires setting an access token. So this identically named
        // subclass of MGLRasterDEMSource swaps in a nonexistent URL.
        class MGLRasterDEMSource: Mapbox.MGLRasterDEMSource {
            override init(identifier: String, configurationURL: URL, tileSize: CGFloat = 256) {
                let bogusURL = URL(string: "https://example.com/raster-rgb.json")!
                super.init(identifier: identifier, configurationURL: bogusURL, tileSize: tileSize)
            }
        }
        
        //#-example-code
        let terrainRGBURL = URL(string: "mapbox://mapbox.terrain-rgb")!
        let source = MGLRasterDEMSource(identifier: "hills", configurationURL: terrainRGBURL)
        mapView.style?.addSource(source)
        //#-end-example-code
        
        XCTAssertNotNil(mapView.style?.source(withIdentifier: "hills"))
    }

    func testMGLVectorTileSource() {
        //#-example-code
        let source = MGLVectorTileSource(identifier: "pois", tileURLTemplates: ["https://example.com/vector-tiles/{z}/{x}/{y}.mvt"], options: [
            .minimumZoomLevel: 9,
            .maximumZoomLevel: 16,
            .attributionInfos: [
                MGLAttributionInfo(title: NSAttributedString(string: "© Mapbox"), url: URL(string: "https://mapbox.com"))
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
        let population = MGLVectorTileSource(identifier: "population", configurationURL: URL(string: "https://example.com/style.json")!)
        mapView.style?.addSource(population)
        
        //#-example-code
        let layer = MGLCircleStyleLayer(identifier: "circles", source: population)
        layer.sourceLayerIdentifier = "population"
        #if os(macOS)
            layer.circleColor = NSExpression(forConstantValue: NSColor.green)
        #else
            layer.circleColor = NSExpression(forConstantValue: UIColor.green)
        #endif
        layer.circleRadius = NSExpression(format: "mgl_interpolate:withCurveType:parameters:stops:($zoomLevel, 'exponential', 1.75, %@)",
                                          [12: 2,
                                           22: 180])
        layer.circleOpacity = NSExpression(forConstantValue: 0.7)
        layer.predicate = NSPredicate(format: "%K == %@", "marital-status", "married")
        mapView.style?.addLayer(layer)
        //#-end-example-code

        XCTAssertNotNil(mapView.style?.layer(withIdentifier: "circles"))
    }

    func testMGLLineStyleLayer() {
        let trails = MGLVectorTileSource(identifier: "trails", configurationURL: URL(string: "https://example.com/style.json")!)
        mapView.style?.addSource(trails)

        //#-example-code
        let layer = MGLLineStyleLayer(identifier: "trails-path", source: trails)
        layer.sourceLayerIdentifier = "trails"
        layer.lineWidth = NSExpression(format: "mgl_interpolate:withCurveType:parameters:stops:($zoomLevel, 'exponential', 1.5, %@)",
                                       [14: 2,
                                        18: 20])
        #if os(macOS)
            layer.lineColor = NSExpression(forConstantValue: NSColor.brown)
        #else
            layer.lineColor = NSExpression(forConstantValue: UIColor.brown)
        #endif
        layer.lineCap = NSExpression(forConstantValue: "round")
        layer.predicate = NSPredicate(format: "%K == %@", "trail-type", "mountain-biking")
        mapView.style?.addLayer(layer)
        //#-end-example-code

        XCTAssertNotNil(mapView.style?.layer(withIdentifier: "trails-path"))
    }

    func testMGLFillStyleLayer() {
        let parks = MGLVectorTileSource(identifier: "parks", configurationURL: URL(string: "https://example.com/style.json")!)
        mapView.style?.addSource(parks)

        //#-example-code
        let layer = MGLFillStyleLayer(identifier: "parks", source: parks)
        layer.sourceLayerIdentifier = "parks"
        #if os(macOS)
            layer.fillColor = NSExpression(forConstantValue: NSColor.green)
        #else
            layer.fillColor = NSExpression(forConstantValue: UIColor.green)
        #endif
        layer.predicate = NSPredicate(format: "type == %@", "national-park")
        mapView.style?.addLayer(layer)
        //#-end-example-code

        XCTAssertNotNil(mapView.style?.layer(withIdentifier: "parks"))
    }
    
    func testMGLFillExtrusionStyleLayer() {
        let buildings = MGLVectorTileSource(identifier: "buildings", configurationURL: URL(string: "https://example.com/style.json")!)
        mapView.style?.addSource(buildings)
        
        //#-example-code
        let layer = MGLFillExtrusionStyleLayer(identifier: "buildings", source: buildings)
        layer.sourceLayerIdentifier = "building"
        layer.fillExtrusionHeight = NSExpression(forKeyPath: "height")
        layer.fillExtrusionBase = NSExpression(forKeyPath: "min_height")
        layer.predicate = NSPredicate(format: "extrude == 'true'")
        mapView.style?.addLayer(layer)
        //#-end-example-code
        
        XCTAssertNotNil(mapView.style?.layer(withIdentifier: "buildings"))
    }
    
    func testMGLHeatmapStyleLayer() {
        let earthquakes = MGLShapeSource(identifier: "earthquakes", url: URL(string: "https://example.com/earthquakes.json")!, options: [:])
        mapView.style?.addSource(earthquakes)
        
        //#-example-code
        let layer = MGLHeatmapStyleLayer(identifier: "earthquake-heat", source: earthquakes)
        layer.heatmapWeight = NSExpression(format: "mgl_interpolate:withCurveType:parameters:stops:(magnitude, 'linear', nil, %@)",
                                           [0: 0,
                                            6: 1])
        layer.heatmapIntensity = NSExpression(format: "mgl_interpolate:withCurveType:parameters:stops:($zoomLevel, 'linear', nil, %@)",
                                              [0: 1,
                                               9: 3])
        mapView.style?.addLayer(layer)
        //#-end-example-code
        
        XCTAssertNotNil(mapView.style?.layer(withIdentifier: "earthquake-heat"))
    }

    func testMGLSymbolStyleLayer() {
        let pois = MGLVectorTileSource(identifier: "pois", configurationURL: URL(string: "https://example.com/style.json")!)
        mapView.style?.addSource(pois)

        //#-example-code
        let layer = MGLSymbolStyleLayer(identifier: "coffeeshops", source: pois)
        layer.sourceLayerIdentifier = "pois"
        layer.iconImageName = NSExpression(forConstantValue: "coffee")
        layer.iconScale = NSExpression(forConstantValue: 0.5)
        layer.text = NSExpression(forKeyPath: "name")
        #if os(macOS)
            var vector = CGVector(dx: 10, dy: 0)
            layer.textTranslation = NSExpression(forConstantValue: NSValue(bytes: &vector, objCType: "{CGVector=dd}"))
        #else
            layer.textTranslation = NSExpression(forConstantValue: NSValue(cgVector: CGVector(dx: 10, dy: 0)))
        #endif
        layer.textJustification = NSExpression(forConstantValue: "left")
        layer.textAnchor = NSExpression(forConstantValue: "left")
        layer.predicate = NSPredicate(format: "%K == %@", "venue-type", "coffee")
        mapView.style?.addLayer(layer)
        //#-end-example-code

        XCTAssertNotNil(mapView.style?.layer(withIdentifier: "coffeeshops"))
    }

    func testMGLRasterStyleLayer() {
        let source = MGLRasterTileSource(identifier: "clouds", tileURLTemplates: ["https://example.com/raster-tiles/{z}/{x}/{y}.png"], options: [
            .minimumZoomLevel: 9,
            .maximumZoomLevel: 16,
            .tileSize: 512,
            .attributionInfos: [
                MGLAttributionInfo(title: NSAttributedString(string: "© Mapbox"), url: URL(string: "https://mapbox.com"))
            ]
        ])
        mapView.style?.addSource(source)

        //#-example-code
        let layer = MGLRasterStyleLayer(identifier: "clouds", source: source)
        layer.rasterOpacity = NSExpression(forConstantValue: 0.5)
        mapView.style?.addLayer(layer)
        //#-end-example-code

        XCTAssertNotNil(mapView.style?.layer(withIdentifier: "clouds"))
    }

    func testMGLHillshadeStyleLayer() {
        let source = MGLRasterDEMSource(identifier: "dem", tileURLTemplates: ["https://example.com/raster-rgb/{z}/{x}/{y}.png"], options: [
            .minimumZoomLevel: 9,
            .maximumZoomLevel: 16,
            .tileSize: 256,
            .attributionInfos: [
                MGLAttributionInfo(title: NSAttributedString(string: "© Mapbox"), url: URL(string: "https://mapbox.com"))
            ]
        ])
        mapView.style?.addSource(source)
        
        let canals = MGLVectorTileSource(identifier: "canals", configurationURL: URL(string: "https://example.com/style.json")!)
        mapView.style?.addSource(canals)
        let canalShadowLayer = MGLLineStyleLayer(identifier: "waterway-river-canal-shadow", source: canals)
        mapView.style?.addLayer(canalShadowLayer)

        //#-example-code
        let layer = MGLHillshadeStyleLayer(identifier: "hills", source: source)
        layer.hillshadeExaggeration = NSExpression(forConstantValue: 0.6)
        if let canalShadowLayer = mapView.style?.layer(withIdentifier: "waterway-river-canal-shadow") {
            mapView.style?.insertLayer(layer, below: canalShadowLayer)
        }
        //#-end-example-code

        XCTAssertNotNil(mapView.style?.layer(withIdentifier: "hills"))
    }

    func testMGLVectorStyleLayer$predicate() {
        let terrain = MGLVectorTileSource(identifier: "terrain", configurationURL: URL(string: "https://example.com/style.json")!)
        mapView.style?.addSource(terrain)

        //#-example-code
        let layer = MGLLineStyleLayer(identifier: "contour", source: terrain)
        layer.sourceLayerIdentifier = "contours"
        layer.predicate = NSPredicate(format: "(index == 5 || index == 10) && CAST(ele, 'NSNumber') >= 1500.0")
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
            static var satelliteStreetsStyleURL: URL {
                return MGLDocumentationExampleTests.styleURL
            }
        }

        //#-example-code
        let camera = MGLMapCamera(lookingAtCenter: CLLocationCoordinate2D(latitude: 37.7184, longitude: -122.4365), altitude: 100, pitch: 20, heading: 0)

        let options = MGLMapSnapshotOptions(styleURL: MGLStyle.satelliteStreetsStyleURL, camera: camera, size: CGSize(width: 320, height: 480))
        options.zoomLevel = 10

        let snapshotter = MGLMapSnapshotter(options: options)
        snapshotter.start { (snapshot, error) in
            if let error = error {
                fatalError(error.localizedDescription)
            }
            
            image = snapshot?.image
        }
        //#-end-example-code
        
        wait(for: [expectation], timeout: 5)
    }
    
    func testMGLCluster() {

        enum ExampleError: Error {
            case unexpectedFeatureType
            case featureIsNotACluster
        }
        
        let geoJSON: [String: Any] = [
            "type" : "Feature",
            "geometry" : [
                "coordinates" : [
                    -77.00896639534831,
                    38.87031006108791,
                    0.0
                ],
                "type" : "Point"
            ],
            "properties" : [
                "cluster" : true,
                "cluster_id" : 123,
                "point_count" : 4567,
            ]
        ]
        
        let clusterShapeData = try! JSONSerialization.data(withJSONObject: geoJSON, options: [])
        
        do {
            //#-example-code
            let shape = try! MGLShape(data: clusterShapeData, encoding: String.Encoding.utf8.rawValue)
            
            guard let pointFeature = shape as? MGLPointFeature else {
                throw ExampleError.unexpectedFeatureType
            }
            
            // Check for cluster conformance
            guard let cluster = pointFeature as? MGLCluster else {
                throw ExampleError.featureIsNotACluster
            }
            
            // Currently the only supported class that conforms to `MGLCluster` is
            // `MGLPointFeatureCluster`
            guard cluster is MGLPointFeatureCluster else {
                throw ExampleError.unexpectedFeatureType
            }
            
            //#-end-example-code
            
            XCTAssert(cluster.clusterIdentifier == 123)
            XCTAssert(cluster.clusterPointCount == 4567)
        }
        catch let error {
            XCTFail("Example failed with thrown error: \(error)")
        }            
    }
    
    func testMGLAttributedExpression() {
        //#-example-code
        #if os(macOS)
        let redColor = NSColor.red
        #else
        let redColor = UIColor.red
        #endif
        let expression = NSExpression(forConstantValue: "Foo")
        let attributes: [MGLAttributedExpressionKey: NSExpression] = [.fontNamesAttribute : NSExpression(forConstantValue: ["DIN Offc Pro Italic",
                                                                                                                            "Arial Unicode MS Regular"]),
                                                                      .fontScaleAttribute: NSExpression(forConstantValue: 1.2),
                                                                      .fontColorAttribute: NSExpression(forConstantValue: redColor)]
        let attributedExpression = MGLAttributedExpression(expression, attributes:attributes)
        //#-end-example-code
        
        XCTAssertNotNil(attributedExpression)
    }
    
    // For testMGLMapView().
    func myCustomFunction() {}
}
