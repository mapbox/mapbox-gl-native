import UIKit
#if swift(>=3)
    import PlaygroundSupport
#else
    import XCPlayground
#endif
import Mapbox

let width: CGFloat = 700
let height: CGFloat = 800

class Responder: NSObject {
    var mapView: MGLMapView?
    func togglePitch(sender: UISwitch)  {
        let camera = mapView!.camera
        #if swift(>=3)
            camera.pitch = sender.isOn ? 60 : 0
        #else
            camera.pitch = sender.on ? 60 : 0
        #endif
        mapView!.setCamera(camera, animated: false)
    }
}

//: A control panel
let panelWidth: CGFloat = 200
let panel = UIView(frame: CGRect(x: width - panelWidth, y: 0, width: 200, height: 100))
panel.alpha = 0.8
#if swift(>=3)
    panel.backgroundColor = .white
#else
    panel.backgroundColor = UIColor.whiteColor()
#endif

// Delete markers
let deleteSwitchLabel = UILabel(frame: CGRect(x: 0, y: 0, width: 100, height: 30))
deleteSwitchLabel.adjustsFontSizeToFitWidth = true
deleteSwitchLabel.text = "Delete Markers"
let deleteMarkerSwitchView = UISwitch(frame: CGRect(x: panelWidth - panelWidth / 2.0, y:0, width: 100, height: 50))
panel.addSubview(deleteSwitchLabel)
panel.addSubview(deleteMarkerSwitchView)

// Hide markers
let hideSwitchLabel = UILabel(frame: CGRect(x: 0, y: 30, width: 100, height: 30))
hideSwitchLabel.adjustsFontSizeToFitWidth = true
hideSwitchLabel.text = "Hide Markers"
let hideMarkerSwitchView = UISwitch(frame: CGRect(x: panelWidth - panelWidth / 2.0, y: 30, width: 100, height: 50))
panel.addSubview(hideSwitchLabel)
panel.addSubview(hideMarkerSwitchView)

// Pitch map
let pitchLabel = UILabel(frame: CGRect(x: 0, y: 60, width: 100, height: 30))
pitchLabel.text = "Pitch"
let pitchSwitch = UISwitch(frame: CGRect(x: panelWidth-panelWidth / 2.0, y: 60, width: 100, height: 50))
let responder = Responder()
#if swift(>=3)
    pitchSwitch.addTarget(responder, action: #selector(responder.togglePitch(sender:)), for: .valueChanged)
#else
    pitchSwitch.addTarget(responder, action: #selector(responder.togglePitch(_:)), forControlEvents: .ValueChanged)
#endif
panel.addSubview(pitchLabel)
panel.addSubview(pitchSwitch)

//: # Mapbox Maps

/*:
 Put your access token into a plain text file called `token`. Then select the “token” placeholder below, go to Editor ‣ Insert File Literal, and select the `token` file.
 */
var accessToken = try String(contentsOfURL: <#token#>)
MGLAccountManager.setAccessToken(accessToken)

class PlaygroundAnnotationView: MGLAnnotationView {
    
    override func prepareForReuse() {
        #if swift(>=3)
            isHidden = hideMarkerSwitchView.isOn
        #else
            hidden = hideMarkerSwitchView.on
        #endif
    }
    
}

//: Define a map delegate

class MapDelegate: NSObject, MGLMapViewDelegate {
    
    var annotationViewByAnnotation = [MGLPointAnnotation: PlaygroundAnnotationView]()
    
    #if swift(>=3)
    func mapView(_ mapView: MGLMapView, viewFor annotation: MGLAnnotation) -> MGLAnnotationView? {
        
        var annotationView = mapView.dequeueReusableAnnotationView(withIdentifier: "annotation") as? PlaygroundAnnotationView
        
        if (annotationView == nil) {
            let av = PlaygroundAnnotationView(reuseIdentifier: "annotation")
            av.frame = CGRect(x: 0, y: 0, width: 30, height: 30)
            av.centerOffset = CGVector(dx: -15, dy: -15)
            let centerView = UIView(frame: av.bounds.insetBy(dx: 3, dy: 3))
            centerView.backgroundColor = .white
            av.addSubview(centerView)
            av.backgroundColor = .purple
            annotationView = av
        } else {
            annotationView!.subviews.first?.backgroundColor = .green
        }
        
        annotationViewByAnnotation[annotation as! MGLPointAnnotation] = annotationView
        
        return annotationView
    }
    #else
    func mapView(mapView: MGLMapView, viewForAnnotation annotation: MGLAnnotation) -> MGLAnnotationView? {
        
        var annotationView = mapView.dequeueReusableAnnotationViewWithIdentifier("annotation") as? PlaygroundAnnotationView
        
        if (annotationView == nil) {
            let av = PlaygroundAnnotationView(reuseIdentifier: "annotation")
            av.frame = CGRect(x: 0, y: 0, width: 30, height: 30)
            av.centerOffset = CGVector(dx: -15, dy: -15)
            let centerView = UIView(frame: CGRectInset(av.bounds, 3, 3))
            centerView.backgroundColor = UIColor.whiteColor()
            av.addSubview(centerView)
            av.backgroundColor = UIColor.purpleColor()
            annotationView = av
        } else {
            annotationView!.subviews.first?.backgroundColor = UIColor.greenColor()
        }
    
        annotationViewByAnnotation[annotation as! MGLPointAnnotation] = annotationView
        
        return annotationView
    }
    #endif
    
    #if swift(>=3)
    func mapView(_ mapView: MGLMapView, didSelect annotation: MGLAnnotation) {
        let pointAnnotation = annotation as! MGLPointAnnotation
        let annotationView: PlaygroundAnnotationView  = annotationViewByAnnotation[pointAnnotation]!
        
        for view in annotationViewByAnnotation.values {
            view.layer.zPosition = -1
        }
        
        annotationView.layer.zPosition = 1
        
        UIView.animate(withDuration: 1.25, delay: 0, usingSpringWithDamping: 0.4, initialSpringVelocity: 0.6, options: .curveEaseOut, animations: {
            annotationView.transform = CGAffineTransform(scaleX: 1.8, y: 1.8)
        }) { _ in
            annotationView.transform = CGAffineTransform(scaleX: 1, y: 1)
            
            if deleteMarkerSwitchView.isOn {
                mapView.removeAnnotation(pointAnnotation)
                return
            }
            
            if hideMarkerSwitchView.isOn {
                annotationView.isHidden = true
            }
        }
    }
    #else
    func mapView(mapView: MGLMapView, didSelectAnnotation annotation: MGLAnnotation) {
        let pointAnnotation = annotation as! MGLPointAnnotation
        let annotationView: PlaygroundAnnotationView  = annotationViewByAnnotation[pointAnnotation]!
        
        for view in annotationViewByAnnotation.values {
            view.layer.zPosition = -1
        }
        
        annotationView.layer.zPosition = 1
        
        UIView.animateWithDuration(1.25, delay: 0, usingSpringWithDamping: 0.4, initialSpringVelocity: 0.6, options: .CurveEaseOut, animations: {
            annotationView.transform = CGAffineTransformMakeScale(1.8, 1.8)
        }) { _ in
            annotationView.transform = CGAffineTransformMakeScale(1, 1)
            
            if deleteMarkerSwitchView.on {
                mapView.removeAnnotation(pointAnnotation)
                return
            }
            
            if hideMarkerSwitchView.on {
                annotationView.hidden = true
            }
        }
    }
    #endif
    
    func handleTap(press: UILongPressGestureRecognizer) {
        let mapView: MGLMapView = press.view as! MGLMapView
        
        #if swift(>=3)
            let isRecognized = press.state == .recognized
        #else
            let isRecognized = press.state == .Recognized
        #endif
        if (isRecognized) {
            #if swift(>=3)
                let coordinate: CLLocationCoordinate2D = mapView.convert(press.location(in: mapView), toCoordinateFrom: mapView)
            #else
                let coordinate: CLLocationCoordinate2D = mapView.convertPoint(press.locationInView(mapView), toCoordinateFromView: mapView)
            #endif
            let annotation = MGLPointAnnotation()
            annotation.title = "Dropped Marker"
            annotation.coordinate = coordinate
            mapView.addAnnotation(annotation)
            mapView.showAnnotations([annotation], animated: true)
        }
    }
    
}

//: Create a map and its delegate

let centerCoordinate = CLLocationCoordinate2D(latitude: 37.174057, longitude: -104.490984)

let mapView = MGLMapView(frame: CGRect(x: 0, y: 0, width: width, height: height))
mapView.frame = CGRect(x: 0, y: 0, width: width, height: height)

#if swift(>=3)
    PlaygroundPage.current.liveView = mapView
#else
    XCPlaygroundPage.currentPage.liveView = mapView
#endif

let mapDelegate = MapDelegate()
mapView.delegate = mapDelegate
responder.mapView = mapView

let tapGesture = UILongPressGestureRecognizer(target: mapDelegate, action: #selector(mapDelegate.handleTap))
mapView.addGestureRecognizer(tapGesture)

//: Zoom in to a location

#if swift(>=3)
    mapView.setCenter(centerCoordinate, zoomLevel: 12, animated: false)
#else
    mapView.setCenterCoordinate(centerCoordinate, zoomLevel: 12, animated: false)
#endif

//: Add control panel

mapView.addSubview(panel)
