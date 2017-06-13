import Mapbox

@objc(PointConversionExample_Swift)

class PointConversionExample_Swift: UIViewController, MGLMapViewDelegate {
    var mapView: MGLMapView!
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        mapView = MGLMapView(frame: view.bounds)
        mapView.autoresizingMask = [.flexibleWidth, .flexibleHeight]
        
        view.addSubview(mapView)
        
        // double tapping zooms the map, so ensure that can still happen
        let doubleTap = UITapGestureRecognizer(target: self, action: nil)
        doubleTap.numberOfTapsRequired = 2
        mapView.addGestureRecognizer(doubleTap)
        
        // delay single tap recognition until it is clearly not a double
        let singleTap = UITapGestureRecognizer(target: self, action: #selector(handleSingleTap))
        singleTap.require(toFail: doubleTap)
        mapView.addGestureRecognizer(singleTap)
        
        // convert `mapView.centerCoordinate` (CLLocationCoordinate2D)
        // to screen location (CGPoint)
        let centerScreenPoint: CGPoint = mapView.convert(mapView.centerCoordinate, toPointTo: mapView)
        print("Screen center: \(centerScreenPoint) = \(mapView.center)")
    }
    
    func handleSingleTap(tap: UITapGestureRecognizer) {
        // convert tap location (CGPoint)
        // to geographic coordinates (CLLocationCoordinate2D)
        let location: CLLocationCoordinate2D = mapView.convert(tap.location(in: mapView), toCoordinateFrom: mapView)
        print("You tapped at: \(location.latitude), \(location.longitude)")
        
        // create an array of coordinates for our polyline
        var coordinates: [CLLocationCoordinate2D] = [mapView.centerCoordinate, location]
        
        // remove existing polyline from the map, (re)add polyline with coordinates
        if (mapView.annotations?.count != nil) {
            mapView.removeAnnotations(mapView.annotations!)
        }

        let polyline = MGLPolyline(coordinates: &coordinates, count: UInt(coordinates.count))

        mapView.addAnnotation(polyline)
    }
}
