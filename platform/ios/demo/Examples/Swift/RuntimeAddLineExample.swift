import Mapbox

@objc(RuntimeAddLineExample_Swift)

class RuntimeAddLineExample_Swift: UIViewController, MGLMapViewDelegate {
    var mapView: MGLMapView!

    override func viewDidLoad() {
        super.viewDidLoad()

        mapView = MGLMapView(frame: view.bounds)
        mapView.autoresizingMask = [.flexibleWidth, .flexibleHeight]

        mapView.setCenter(
            CLLocationCoordinate2D(latitude: 45.5076, longitude: -122.6736),
            zoomLevel: 11,
            animated: false)
        view.addSubview(mapView)

        mapView.delegate = self
    }

    // Wait until the map is loaded before adding to the map.
    func mapView(_ mapView: MGLMapView, didFinishLoading style: MGLStyle) {
        loadGeoJson()
    }

    func loadGeoJson() {
        DispatchQueue.global().async {
            // Get the path for example.geojson in the app’s bundle.
            guard let jsonUrl = Bundle.main.url(forResource: "example", withExtension: "geojson") else { return }
            guard let jsonData = try? Data(contentsOf: jsonUrl) else { return }
            DispatchQueue.main.async {
                self.drawPolyline(geoJson: jsonData)
            }
        }
    }

    func drawPolyline(geoJson: Data) {
        // Add our GeoJSON data to the map as an MGLGeoJSONSource.
        // We can then reference this data from an MGLStyleLayer.

        // MGLMapView.style is optional, so you must guard against it not being set.
        guard let style = self.mapView.style else { return }

        let shapeFromGeoJSON = try! MGLShape(data: geoJson, encoding: String.Encoding.utf8.rawValue)
        let source = MGLShapeSource(identifier: "polyline", shape: shapeFromGeoJSON, options: nil)
        style.addSource(source)

        // Create new layer for the line
        let layer = MGLLineStyleLayer(identifier: "polyline", source: source)
        layer.lineJoin = MGLStyleValue(rawValue: NSValue(mglLineJoin: .round))
        layer.lineCap = MGLStyleValue(rawValue: NSValue(mglLineCap: .round))
        layer.lineColor = MGLStyleValue(rawValue: UIColor(red: 59/255, green:178/255, blue:208/255, alpha:1))
        // Use a style function to smoothly adjust the line width from 2pt to 20pt between zoom levels 14 and 18. The `interpolationBase` parameter allows the values to interpolate along an exponential curve.
        layer.lineWidth = MGLStyleValue(interpolationMode: .exponential,
            cameraStops: [14: MGLStyleValue<NSNumber>(rawValue: 2),
                          18: MGLStyleValue<NSNumber>(rawValue: 20)],
            options: [.defaultValue : MGLConstantStyleValue<NSNumber>(rawValue: 1.5)])

        // We can also add a second layer that will draw a stroke around the original line.
        let casingLayer = MGLLineStyleLayer(identifier: "polyline-case", source: source)
        // Copy these attributes from the main line layer.
        casingLayer.lineJoin = layer.lineJoin
        casingLayer.lineCap = layer.lineCap
        // Line gap width represents the space before the outline begins, so should match the main line’s line width exactly.
        casingLayer.lineGapWidth = layer.lineWidth
        // Stroke color slightly darker than the line color.
        casingLayer.lineColor = MGLStyleValue(rawValue: UIColor(red: 41/255, green:145/255, blue:171/255, alpha:1))
        // Use a style function to gradually increase the stroke width between zoom levels 14 and 18.
        casingLayer.lineWidth = MGLStyleValue(interpolationMode: .exponential,
            cameraStops: [14: MGLStyleValue(rawValue: 1),
                          18: MGLStyleValue(rawValue: 4)],
            options: [.defaultValue : MGLConstantStyleValue<NSNumber>(rawValue: 1.5)])

        // Just for fun, let’s add another copy of the line with a dash pattern.
        let dashedLayer = MGLLineStyleLayer(identifier: "polyline-dash", source: source)
        dashedLayer.lineJoin = layer.lineJoin
        dashedLayer.lineCap = layer.lineCap
        dashedLayer.lineColor = MGLStyleValue(rawValue: .white)
        dashedLayer.lineOpacity = MGLStyleValue(rawValue: 0.5)
        dashedLayer.lineWidth = layer.lineWidth
        // Dash pattern in the format [dash, gap, dash, gap, ...]. You’ll want to adjust these values based on the line cap style.
        dashedLayer.lineDashPattern = MGLStyleValue(rawValue: [0, 1.5])
        
        style.addLayer(layer)
        style.addLayer(dashedLayer)
        style.insertLayer(casingLayer, below: layer)
    }
}
