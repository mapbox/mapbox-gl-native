import XCTest
import Mapbox

/**
 Test cases that ensure the inline examples in the project documentation
 compile. If any of these need to be changed, update the corresponding header
 documentation accordingly.
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

    // Example code in MGLShapeSource
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

    // Example code in MGLRasterSource
    func testMGLRasterSourceExample() {
	let source = MGLRasterSource(identifier: "clouds", tileURLTemplates: ["https://example.com/raster-tiles/{z}/{x}/{y}.png"], options: [
	    .minimumZoomLevel: 9,
	    .maximumZoomLevel: 16,
	    .tileSize: 512,
	    .attributionInfos: [
		MGLAttributionInfo(title: NSAttributedString(string: "© Mapbox"), url: URL(string: "http://mapbox.com"))
	    ]
	])
	mapView.style.addSource(source)

	XCTAssertNotNil(mapView.style.source(withIdentifier: "clouds"))
    }

    // Example code in MGLVectorSource
    func testMGLVectorSource() {
	let source = MGLVectorSource(identifier: "pois", tileURLTemplates: ["https://example.com/vector-tiles/{z}/{x}/{y}.mvt"], options: [
	    .minimumZoomLevel: 9,
	    .maximumZoomLevel: 16,
	    .attributionInfos: [
		MGLAttributionInfo(title: NSAttributedString(string: "© Mapbox"), url: URL(string: "http://mapbox.com"))
	    ]
	])
	mapView.style.addSource(source)

	XCTAssertNotNil(mapView.style.source(withIdentifier: "pois"))
    }

    // Example code in MGLCircleStyleLayer
    func testMGLCircleStyleLayerExample() {
	// Don't bother with the source setup in the header docs
	let population = MGLVectorSource(identifier: "population", configurationURL: URL(string: "https://example.com/style.json")!)
	mapView.style.addSource(population)

	// Start documentation here
	let layer = MGLCircleStyleLayer(identifier: "circles", source: population)
	layer.sourceLayerIdentifier = "population"
	layer.circleColor = MGLStyleValue(rawValue: UIColor.green)
	layer.circleRadius = MGLStyleValue(interpolationBase: 1.75, stops: [
	    12: MGLStyleValue(rawValue: 2),
	    22: MGLStyleValue(rawValue: 180)
	])
	layer.circleOpacity = MGLStyleValue(rawValue: 0.7)
	layer.predicate = NSPredicate(format: "%K == %@", "marital-status", "married")
	mapView.style.addLayer(layer)

	XCTAssertNotNil(mapView.style.layer(withIdentifier: "circles"))
    }

    // Example code in MGLLineStyleLayer
    func testMGLLineStyleLayerExample() {
	let trails = MGLVectorSource(identifier: "trails", configurationURL: URL(string: "https://example.com/style.json")!)
	mapView.style.addSource(trails)

	let layer = MGLLineStyleLayer(identifier: "trails-path", source: trails)
	layer.sourceLayerIdentifier = "trails"
	layer.lineWidth = MGLStyleValue(interpolationBase: 1.5, stops: [
	    14: MGLStyleValue(rawValue: 2),
	    18: MGLStyleValue(rawValue: 20),
	])
	layer.lineColor = MGLStyleValue(rawValue: UIColor.brown)
	layer.lineCap = MGLStyleValue(rawValue: NSValue(mglLineCap: .round))
	layer.predicate = NSPredicate(format: "%K == %@", "trail-type", "mountain-biking")
	mapView.style.addLayer(layer)

	XCTAssertNotNil(mapView.style.layer(withIdentifier: "trails-path"))
    }

    // Example code in MGLFillStyleLayer
    func testMGLFillStyleLayerExample() {
	let parks = MGLVectorSource(identifier: "parks", configurationURL: URL(string: "https://example.com/style.json")!)
	mapView.style.addSource(parks)

	let layer = MGLFillStyleLayer(identifier: "parks", source: parks)
	layer.sourceLayerIdentifier = "parks"
	layer.fillColor = MGLStyleValue(rawValue: UIColor.green)
	layer.predicate = NSPredicate(format: "type == %@", "national-park")
	mapView.style.addLayer(layer)

	XCTAssertNotNil(mapView.style.layer(withIdentifier: "parks"))
    }

    // Example code in MGLSymbolStyleLayer
    func testMGLSymbolStyleLayerExample() {
	let pois = MGLVectorSource(identifier: "pois", configurationURL: URL(string: "https://example.com/style.json")!)
	mapView.style.addSource(pois)

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

	XCTAssertNotNil(mapView.style.layer(withIdentifier: "coffeeshops"))
    }

    // Example code in MGLVectorStyleLayer.predicate
    func testMGLVectorStyleLayer() {
	let terrain = MGLVectorSource(identifier: "terrain", configurationURL: URL(string: "https://example.com/style.json")!)
	mapView.style.addSource(terrain)

	let layer = MGLLineStyleLayer(identifier: "contour", source: terrain)
	layer.sourceLayerIdentifier = "contours"
	layer.predicate = NSPredicate(format: "(index == 10 || index == 5) && ele >= 1500.0")
	mapView.style.addLayer(layer)

	XCTAssertNotNil(mapView.style.layer(withIdentifier: "contour"))
    }
}
