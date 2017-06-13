import Mapbox
    
@objc(DrawingACustomMarkerExample_Swift)

class DrawingACustomMarkerExample_Swift: UIViewController, MGLMapViewDelegate {
    override func viewDidLoad() {
        super.viewDidLoad()
        
        let mapView = MGLMapView(frame: view.bounds, styleURL: MGLStyle.lightStyleURL(withVersion: 9))
        mapView.autoresizingMask = [.flexibleWidth, .flexibleHeight]
        mapView.tintColor = .darkGray
        
        // Set the map's bounds to Pisa, Italy.
        let bounds = MGLCoordinateBounds(
            sw: CLLocationCoordinate2D(latitude: 43.7115, longitude: 10.3725),
            ne: CLLocationCoordinate2D(latitude: 43.7318, longitude: 10.4222))
        mapView.setVisibleCoordinateBounds(bounds, animated: false)
        
        view.addSubview(mapView)
        
        // Set the map view‘s delegate property.
        mapView.delegate = self
        
        // Initialize and add the point annotation.
        let pisa = MGLPointAnnotation()
        pisa.coordinate = CLLocationCoordinate2D(latitude: 43.72305, longitude: 10.396633)
        pisa.title = "Leaning Tower of Pisa"
        mapView.addAnnotation(pisa)
    }
    
    func mapView(_ mapView: MGLMapView, imageFor annotation: MGLAnnotation) -> MGLAnnotationImage? {
        // Try to reuse the existing ‘pisa’ annotation image, if it exists.
        var annotationImage = mapView.dequeueReusableAnnotationImage(withIdentifier: "pisa")
        
        if annotationImage == nil {
            // Leaning Tower of Pisa by Stefan Spieler from the Noun Project.
            var image = UIImage(named: "pisavector")!
            
            // The anchor point of an annotation is currently always the center. To
            // shift the anchor point to the bottom of the annotation, the image
            // asset includes transparent bottom padding equal to the original image
            // height.
            //
            // To make this padding non-interactive, we create another image object
            // with a custom alignment rect that excludes the padding.
            image = image.withAlignmentRectInsets(UIEdgeInsets(top: 0, left: 0, bottom: image.size.height/2, right: 0))
            
            // Initialize the ‘pisa’ annotation image with the UIImage we just loaded.
            annotationImage = MGLAnnotationImage(image: image, reuseIdentifier: "pisa")
        }
        
        return annotationImage
    }
    
    func mapView(_ mapView: MGLMapView, annotationCanShowCallout annotation: MGLAnnotation) -> Bool {
        // Always allow callouts to popup when annotations are tapped.
        return true
    }
}
