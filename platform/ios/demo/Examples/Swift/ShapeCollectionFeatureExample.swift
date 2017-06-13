import Mapbox;

@objc(ShapeCollectionFeatureExample_Swift)

class ShapeCollectionFeatureExample_Swift: UIViewController, MGLMapViewDelegate {
    
    var mapView: MGLMapView!

    override func viewDidLoad() {
        super.viewDidLoad()
        
        mapView = MGLMapView(frame: view.bounds)
        mapView.styleURL = MGLStyle.lightStyleURL(withVersion: 9)
        mapView.autoresizingMask = [.flexibleWidth, .flexibleHeight]
        mapView.tintColor = .darkGray
        
        mapView.setCenter(CLLocationCoordinate2D(latitude:38.897435, longitude: -77.039679), zoomLevel: 12, animated: false)
        
        mapView.delegate = self
        view.addSubview(mapView)
    }
    
    func mapView(_ mapView: MGLMapView, didFinishLoading style: MGLStyle) {
        
        // Parse the GeoJSON data.
        DispatchQueue.global().async {
            guard let url = Bundle.main.url(forResource: "metro-line", withExtension: "geojson") else { return }

            let data = try! Data(contentsOf: url)
            
            DispatchQueue.main.async {
                self.drawShapeCollection(data: data)
            }
        }
    }
    
    func drawShapeCollection(data: Data) {
        guard let style = self.mapView.style else { return }
        
        // Use [MGLShape shapeWithData:encoding:error:] to create a MGLShapeCollectionFeature from GeoJSON data.
        let feature = try! MGLShape(data: data, encoding: String.Encoding.utf8.rawValue) as! MGLShapeCollectionFeature
        
        // Create source and add it to the map style.
        let source = MGLShapeSource(identifier: "transit", shape: feature, options: nil)
        style.addSource(source)
        
        // Create station style layer.
        let circleLayer = MGLCircleStyleLayer(identifier: "stations", source: source)
        
        // Use a predicate to filter out non-points.
        circleLayer.predicate = NSPredicate(format: "TYPE = 'Station'")
        circleLayer.circleColor = MGLStyleValue(rawValue: .red)
        circleLayer.circleRadius = MGLStyleValue(rawValue: 6)
        circleLayer.circleStrokeWidth = MGLStyleValue(rawValue: 2)
        circleLayer.circleStrokeColor = MGLStyleValue(rawValue: .black)
        
        // Create line style layer.
        let lineLayer = MGLLineStyleLayer(identifier: "rail-line", source: source)
        
        // Use a predicate to filter out the stations.
        lineLayer.predicate = NSPredicate(format: "TYPE = 'Rail line'")
        lineLayer.lineColor = MGLStyleValue(rawValue: .red)
        lineLayer.lineWidth = MGLStyleValue(rawValue: 2)
        
        // Add style layers to the map view's style.
        style.addLayer(circleLayer)
        style.insertLayer(lineLayer, below: circleLayer)
    }
}
