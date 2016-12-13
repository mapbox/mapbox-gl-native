import XCTest
import Mapbox

/**
 Test cases that ensure the inline examples in the project documentation
 compile. If any of these need to be changed, update the corresponding header
 documentation accordingly.
 */
class MBLDocumentationExampleTests: XCTestCase {
    var mapView: MGLMapView!

    override func setUp() {
	super.setUp()
	self.mapView = MGLMapView(frame: CGRect(x: 0, y: 0, width: 256, height: 256))
    }

    override func tearDown() {
	self.mapView = nil
	super.tearDown()
    }

    // Example code in MGLShapeSource.h
    func testMGLShapeSourceExample() {
	var coordinates: [CLLocationCoordinate2D] = [
	    CLLocationCoordinate2D(latitude: 37.77, longitude: -122.42),
	    CLLocationCoordinate2D(latitude: 38.91, longitude: -77.04),
	    ]
	let polyline = MGLPolylineFeature(coordinates: &coordinates, count: UInt(coordinates.count))
	let shape = MGLShapeCollectionFeature(shapes: [polyline])
	let source = MGLShapeSource(identifier: "lines", shape: shape, options: nil)
	mapView.style.addSource(source)

	XCTAssertNotNil(mapView.style.source(withIdentifier: "lines"))
    }

    // Example code in MGLRasterSource.h
    func testMGLRasterSourceExample() {
	// Define tileset
	let tileset = MGLTileSet(tileURLTemplates: ["https://example.com/raster-tiles/{z}/{x}/{y}.png"])
	tileset.minimumZoomLevel = 9
	tileset.maximumZoomLevel = 16
	tileset.attribution = "© Mapbox"
	// Add source to map
	let source = MGLRasterSource(identifier: "clouds", tileSet: tileset, tileSize: 512)
	mapView.style.addSource(source)

	XCTAssertNotNil(mapView.style.source(withIdentifier: "clouds"))
    }

    // Example code in MGLVectorSource.h
    func testMGLVectorSource() {
	// Define tileset
	let tileset = MGLTileSet(tileURLTemplates: ["https://example.com/vector-tiles/{z}/{x}/{y}.mvt"])
	tileset.minimumZoomLevel = 9
	tileset.maximumZoomLevel = 16
	tileset.attribution = "© Mapbox"
	// Add source to map
	let source = MGLVectorSource(identifier: "pois", tileSet: tileset)
	mapView.style.addSource(source)

	XCTAssertNotNil(mapView.style.source(withIdentifier: "pois"))
    }

    // Example code in MGLCircleStyleLayer.h
    func testMGLCircleStyleLayerExample() {
	let population = MGLVectorSource(identifier: "population", url: URL(string: "https://example.com")!)
	mapView.style.addSource(population)

	let layer = MGLCircleStyleLayer(identifier: "circles", source: population)
	layer.sourceLayerIdentifier = "population"
	layer.circleColor = MGLStyleValue(rawValue: UIColor.green)
	layer.circleRadius = MGLStyleValue(base: 1.75, stops: [
	    12: MGLStyleValue(rawValue: 2),
	    22: MGLStyleValue(rawValue: 180)
	    ])
	layer.circleOpacity = MGLStyleValue(rawValue: 0.7)
	layer.predicate = NSPredicate(format: "%K == %@", "marital-status", "married")
	mapView.style.addLayer(layer)

	XCTAssertNotNil(mapView.style.layer(withIdentifier: "circles"))
    }

    // Example code in MGLLineStyleLayer.h
    func testMGLLineStyleLayerExample() {
	let trails = MGLVectorSource(identifier: "trails", url: URL(string: "https://example.com")!)
	mapView.style.addSource(trails)

	let layer = MGLLineStyleLayer(identifier: "trails-path", source: trails)
	layer.sourceLayerIdentifier = "trails"
	layer.lineWidth = MGLStyleValue(base: 1.5, stops: [
	    14: MGLStyleValue(rawValue: 2),
	    18: MGLStyleValue(rawValue: 20),
	    ])
	layer.lineColor = MGLStyleValue(rawValue: UIColor.brown)
	layer.lineCap = MGLStyleValue(rawValue: NSValue(mglLineCap: .round))
	layer.predicate = NSPredicate(format: "%K == %@", "trail-type", "mountain-biking")
	mapView.style.addLayer(layer)

	XCTAssertNotNil(mapView.style.layer(withIdentifier: "trails-path"))
    }

    // Example code in MGLFillStyleLayer.h
    func testMGLFillStyleLayerExample() {
	let parks = MGLVectorSource(identifier: "parks", url: URL(string: "https://example.com")!)
	mapView.style.addSource(parks)

	let layer = MGLFillStyleLayer(identifier: "parks", source: parks)
	layer.sourceLayerIdentifier = "parks"
	layer.fillColor = MGLStyleValue(rawValue: UIColor.green)
	layer.predicate = NSPredicate(format: "type == %@", "national-park")
	mapView.style.addLayer(layer)

	XCTAssertNotNil(mapView.style.layer(withIdentifier: "parks"))
    }

    // Example code in MGLSymbolStyleLayer.h
    func testMGLSymbolStyleLayerExample() {
	let pois = MGLVectorSource(identifier: "pois", url: URL(string: "https://example.com")!)
	mapView.style.addSource(pois)

	let layer = MGLSymbolStyleLayer(identifier: "coffeeshops", source: pois)
	layer.sourceLayerIdentifier = "pois"
	layer.iconImageName = MGLStyleValue(rawValue: "coffee")
	layer.iconScale = MGLStyleValue(rawValue: 0.5)
	layer.textField = MGLStyleValue(rawValue: "{name}")
	layer.textTranslate = MGLStyleValue(rawValue: NSValue(cgVector: CGVector(dx: 10, dy: 0)))
	layer.textJustify = MGLStyleValue(rawValue: NSValue(mglTextJustify: .left))
	layer.textAnchor = MGLStyleValue(rawValue: NSValue(mglTextAnchor: .left))
	layer.predicate = NSPredicate(format: "%K == %@", "venue-type", "coffee")
	mapView.style.addLayer(layer)

	XCTAssertNotNil(mapView.style.layer(withIdentifier: "coffeeshops"))
    }

    // Example code in MGLVectorStyleLayer.h
    func testMGLVectorStyleLayer() {
	let terrain = MGLVectorSource(identifier: "terrain", url: URL(string: "https://example.com")!)
	mapView.style.addSource(terrain)

	let layer = MGLLineStyleLayer(identifier: "contour", source: terrain)
	layer.sourceLayerIdentifier = "contours"
	layer.predicate = NSPredicate(format: "(index == 10 || index == 5) && ele >= 1500.0")
	mapView.style.addLayer(layer)

	XCTAssertNotNil(mapView.style.layer(withIdentifier: "contour"))
    }
}
