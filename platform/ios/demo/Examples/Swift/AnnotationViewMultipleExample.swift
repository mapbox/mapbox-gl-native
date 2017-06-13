import Mapbox

// MGLPointAnnotation subclass
class MyCustomPointAnnotation: MGLPointAnnotation {
    var willUseImage: Bool = false
}
// end MGLPointAnnotation subclass

@objc(AnnotationViewMultipleExample_Swift)

class AnnotationViewMultipleExample_Swift: UIViewController, MGLMapViewDelegate {
    override func viewDidLoad() {
        super.viewDidLoad()
        
        // Create a new map view using the Mapbox Light style.
        let mapView = MGLMapView(frame: view.bounds, styleURL: MGLStyle.lightStyleURL(withVersion: 9))
        
        mapView.autoresizingMask = [.flexibleWidth, .flexibleHeight]
        
        // Set the map’s center coordinate and zoom level.
        mapView.setCenter(CLLocationCoordinate2D(latitude: 36.54, longitude: -116.97), zoomLevel: 9, animated: false)
        view.addSubview(mapView)
        mapView.delegate = self

       // Create four new point annotations with specified coordinates and titles.
        let pointA = MyCustomPointAnnotation()
        pointA.coordinate = CLLocationCoordinate2D(latitude: 36.4623, longitude: -116.8656)
        pointA.title = "Stovepipe Wells"
        pointA.willUseImage = true

        let pointB = MyCustomPointAnnotation()
        pointB.coordinate = CLLocationCoordinate2D(latitude: 36.6071, longitude: -117.1458)
        pointB.title = "Furnace Creek"
        pointB.willUseImage = true

        let pointC = MyCustomPointAnnotation()
        pointC.title = "Zabriskie Point"
        pointC.coordinate = CLLocationCoordinate2D(latitude: 36.4208, longitude: -116.8101)
        
        let pointD = MyCustomPointAnnotation()
        pointD.title = "Mesquite Flat Sand Dunes"
        pointD.coordinate = CLLocationCoordinate2D(latitude: 36.6836, longitude: -117.1005)
        
        // Fill an array with four point annotations.
        let myPlaces = [pointA, pointB, pointC, pointD]
        
        // Add all annotations to the map all at once, instead of individually.
        mapView.addAnnotations(myPlaces)
        
    }
    
    // This delegate method is where you tell the map to load a view for a specific annotation based on the willUseImage property of the custom subclass.
    func mapView(_ mapView: MGLMapView, viewFor annotation: MGLAnnotation) -> MGLAnnotationView? {
        
        if let castAnnotation = annotation as? MyCustomPointAnnotation {
            if (castAnnotation.willUseImage) {
                return nil;
            }
        }
        
        // Assign a reuse identifier to be used by both of the annotation views, taking advantage of their similarities.
        let reuseIdentifier = "reusableDotView"
        
        // For better performance, always try to reuse existing annotations.
        var annotationView = mapView.dequeueReusableAnnotationView(withIdentifier: reuseIdentifier)
        
        // If there’s no reusable annotation view available, initialize a new one.
        if annotationView == nil {
            annotationView = MGLAnnotationView(reuseIdentifier: reuseIdentifier)
            annotationView?.frame = CGRect(x: 0, y: 0, width: 30, height: 30)
            annotationView?.layer.cornerRadius = (annotationView?.frame.size.width)! / 2
            annotationView?.layer.borderWidth = 4.0
            annotationView?.layer.borderColor = UIColor.white.cgColor
            annotationView!.backgroundColor = UIColor(red:0.03, green:0.80, blue:0.69, alpha:1.0)
        }
        
        return annotationView
    }
    
    // This delegate method is where you tell the map to load an image for a specific annotation based on the willUseImage property of the custom subclass.
    func mapView(_ mapView: MGLMapView, imageFor annotation: MGLAnnotation) -> MGLAnnotationImage? {
        
        if let castAnnotation = annotation as? MyCustomPointAnnotation {
            if (!castAnnotation.willUseImage) {
                return nil;
            }
        }
        
        // For better performance, always try to reuse existing annotations.
        var annotationImage = mapView.dequeueReusableAnnotationImage(withIdentifier: "camera")
        
        // If there is no reusable annotation image available, initialize a new one.
        if(annotationImage == nil) {
            annotationImage = MGLAnnotationImage(image: UIImage(named: "camera")!, reuseIdentifier: "camera")
        }
        
        return annotationImage
    }
    
    func mapView(_ mapView: MGLMapView, annotationCanShowCallout annotation: MGLAnnotation) -> Bool {
        // Always allow callouts to popup when annotations are tapped.
        return true
    }
    
}
