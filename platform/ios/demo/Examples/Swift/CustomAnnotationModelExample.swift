import Mapbox

@objc(CustomAnnotationModelExample_Swift)

class CustomAnnotationModelExample_Swift: UIViewController, MGLMapViewDelegate {
    override func viewDidLoad() {
        super.viewDidLoad()
        
        let mapView = MGLMapView(frame: view.bounds)
        mapView.autoresizingMask = [.flexibleWidth, .flexibleHeight]
        mapView.styleURL = MGLStyle.lightStyleURL(withVersion: 9)
        mapView.tintColor = .darkGray
        mapView.zoomLevel = 1
        mapView.delegate = self
        view.addSubview(mapView)
        
        // Polyline
        // Create a coordinates array with all of the coordinates for our polyline.
        var coordinates = [
            CLLocationCoordinate2D(latitude: 35,  longitude: -25),
            CLLocationCoordinate2D(latitude: 20,  longitude: -30),
            CLLocationCoordinate2D(latitude: 0,   longitude: -25),
            CLLocationCoordinate2D(latitude: -15, longitude: 0),
            CLLocationCoordinate2D(latitude: -45, longitude: 10),
            CLLocationCoordinate2D(latitude: -45, longitude: 40),
        ]
        
        let polyline = CustomPolyline(coordinates: &coordinates, count: UInt(coordinates.count))

        // Set the custom `color` property, later used in the `mapView:strokeColorForShapeAnnotation:` delegate method.
        polyline.color = .darkGray
        
        // Add the polyline to the map. Note that this method name is singular.
        mapView.addAnnotation(polyline)
        
        // Point Annotations
        // Add a custom point annotation for every coordinate (vertex) in the polyline.
        var pointAnnotations = [CustomPointAnnotation]()
        for coordinate in coordinates {
            let count = pointAnnotations.count + 1
            let point = CustomPointAnnotation(coordinate: coordinate,
                                              title: "Custom Point Annotation \(count)",
                                              subtitle: nil)
            
            // Set the custom `image` and `reuseIdentifier` properties, later used in the `mapView:imageForAnnotation:` delegate method.
            // Create a unique reuse identifier for each new annotation image.
            point.reuseIdentifier = "customAnnotation\(count)"
            // This dot image grows in size as more annotations are added to the array.
            point.image = dot(size:5 * count)
            
            // Append each annotation to the array, which will be added to the map all at once.
            pointAnnotations.append(point)
        }
        
        // Add the point annotations to the map. This time the method name is plural.
        // If you have multiple annotations to add, batching their addition to the map is more efficient.
        mapView.addAnnotations(pointAnnotations)
    }
    
    func dot(size: Int) -> UIImage {
        let floatSize = CGFloat(size)
        let rect = CGRect(x: 0, y: 0, width: floatSize, height: floatSize)
        let strokeWidth: CGFloat = 1
        
        UIGraphicsBeginImageContextWithOptions(rect.size, false, UIScreen.main.scale)
        
        let ovalPath = UIBezierPath(ovalIn: rect.insetBy(dx: strokeWidth, dy: strokeWidth))
        UIColor.darkGray.setFill()
        ovalPath.fill()
        
        UIColor.white.setStroke()
        ovalPath.lineWidth = strokeWidth
        ovalPath.stroke()
        
        let image: UIImage = UIGraphicsGetImageFromCurrentImageContext()!
        UIGraphicsEndImageContext()
        
        return image
    }
    
    // MARK: - MGLMapViewDelegate methods
    
    func mapView(_ mapView: MGLMapView, imageFor annotation: MGLAnnotation) -> MGLAnnotationImage? {
        if let point = annotation as? CustomPointAnnotation,
            let image = point.image,
            let reuseIdentifier = point.reuseIdentifier {
            
            if let annotationImage = mapView.dequeueReusableAnnotationImage(withIdentifier: reuseIdentifier) {
                // The annotatation image has already been cached, just reuse it.
                return annotationImage
            } else {
                // Create a new annotation image.
                return MGLAnnotationImage(image: image, reuseIdentifier: reuseIdentifier)
            }
        }
        
        // Fallback to the default marker image.
        return nil
    }
    
    func mapView(_ mapView: MGLMapView, strokeColorForShapeAnnotation annotation: MGLShape) -> UIColor {
        if let annotation = annotation as? CustomPolyline {
            // Return orange if the polyline does not have a custom color.
            return annotation.color ?? .orange
        }
        
        // Fallback to the default tint color.
        return mapView.tintColor
    }
    
    func mapView(_ mapView: MGLMapView, annotationCanShowCallout annotation: MGLAnnotation) -> Bool {
        return true
    }
}
