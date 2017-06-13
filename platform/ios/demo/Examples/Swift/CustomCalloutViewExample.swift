import Mapbox

@objc(CustomCalloutViewExample_Swift)

class CustomCalloutViewExample_Swift: UIViewController, MGLMapViewDelegate {
    override func viewDidLoad() {
        super.viewDidLoad()
        
        let mapView = MGLMapView(frame: view.bounds, styleURL: MGLStyle.lightStyleURL(withVersion: 9))
        mapView.autoresizingMask = [.flexibleWidth, .flexibleHeight]
        mapView.tintColor = .darkGray
        view.addSubview(mapView)
        
        // Set the map view‘s delegate property.
        mapView.delegate = self
        
        // Initialize and add the marker annotation.
        let marker = MGLPointAnnotation()
        marker.coordinate = CLLocationCoordinate2D(latitude: 0, longitude: 0)
        marker.title = "Hello world!"
        
        // This custom callout example does not implement subtitles.
        //marker.subtitle = "Welcome to my marker"
        
        // Add marker to the map.
        mapView.addAnnotation(marker)
    }
    
    func mapView(_ mapView: MGLMapView, annotationCanShowCallout annotation: MGLAnnotation) -> Bool {
        // Always allow callouts to popup when annotations are tapped.
        return true
    }

    func mapView(_ mapView: MGLMapView, calloutViewFor annotation: MGLAnnotation) -> MGLCalloutView? {
        // Only show callouts for `Hello world!` annotation.
        if annotation.responds(to: #selector(getter: MGLAnnotation.title)) && annotation.title! == "Hello world!" {
            // Instantiate and return our custom callout view.
            return CustomCalloutView(representedObject: annotation)
        }
        
        return nil
    }
    
    func mapView(_ mapView: MGLMapView, tapOnCalloutFor annotation: MGLAnnotation) {
        // Optionally handle taps on the callout.
        print("Tapped the callout for: \(annotation)")
        
        // Hide the callout.
        mapView.deselectAnnotation(annotation, animated: true)
    }
}
