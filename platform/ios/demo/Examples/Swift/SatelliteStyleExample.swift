import Mapbox

@objc(SatelliteStyleExample_Swift)

class SatelliteStyleExample_Swift: UIViewController {
    var mapView: MGLMapView!
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        // A hybrid style with unobtrusive labels is also available via satelliteStreetsStyleURL(withVersion:).
        mapView = MGLMapView(frame: view.bounds, styleURL: MGLStyle.satelliteStyleURL(withVersion: 9))
        
        // Tint the ℹ️ button.
        mapView.attributionButton.tintColor = .white
        
        mapView.autoresizingMask = [.flexibleWidth, .flexibleHeight]
        
        // Set the map’s center coordinate and zoom level.
        mapView.setCenter(CLLocationCoordinate2D(latitude: 45.5188, longitude: -122.6748), zoomLevel: 13, animated: false)
        
        view.addSubview(mapView)
    }
    
}
