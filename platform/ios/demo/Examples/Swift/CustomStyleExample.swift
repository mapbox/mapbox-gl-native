import Mapbox

@objc(CustomStyleExample_Swift)

class CustomStyleExample_Swift: UIViewController {
    override func viewDidLoad() {
        super.viewDidLoad()
        
        // Fill in the next line with your style URL from Mapbox Studio.
        // <#mapbox://styles/userName/styleHash#>
        let styleURL = URL(string: "mapbox://styles/mapbox/outdoors-v9")
        let mapView = MGLMapView(frame: view.bounds,
                                 styleURL: styleURL)
        mapView.autoresizingMask = [.flexibleWidth, .flexibleHeight]
        
        // Set the map’s center coordinate and zoom level.
        mapView.setCenter(CLLocationCoordinate2D(latitude: 45.52954,
                longitude: -122.72317),
                zoomLevel: 14, animated: false)
        view.addSubview(mapView)
    }
}
