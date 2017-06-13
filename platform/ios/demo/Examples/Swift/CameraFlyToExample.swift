import Mapbox

@objc(CameraFlyToExample_Swift)

class CameraFlyToExample_Swift: UIViewController, MGLMapViewDelegate {
    var mapView: MGLMapView!
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        mapView = MGLMapView(frame: view.bounds)
        mapView.autoresizingMask = [.flexibleWidth, .flexibleHeight]
        
        // Sets Honolulu, Hawaii as the camera's starting point.
        let honolulu = CLLocationCoordinate2D(latitude: 21.3069,
                                              longitude: -157.8583)
        mapView.setCenter(honolulu,
                          zoomLevel:14, animated: false)
        
        mapView.delegate = self
        view.addSubview(mapView)
    }
    
    func mapViewDidFinishLoadingMap(_ mapView: MGLMapView) {
        
        // Waits for the mapView to finish loading before setting up the camera.
        // Defines the destination camera as Hawaii Island.
        let camera = MGLMapCamera(lookingAtCenter:
            CLLocationCoordinate2D(latitude: 19.784213, longitude: -155.784605),
                                  fromDistance: 35000, pitch: 70, heading: 90)
        
        // Goes from Honolulu to destination camera.
        mapView.fly(to: camera, withDuration: 4,
                    peakAltitude: 3000, completionHandler: nil)
        // To use default duration and peak altitudes:
        //    mapView.fly(to: camera, completionHandler: nil)
        // To use default peak altitude:
        //    mapView.fly(to: camera, withDuration: 4, completionHandler: nil)
    }
}
