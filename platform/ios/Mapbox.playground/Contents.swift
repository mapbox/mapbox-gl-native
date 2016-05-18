import UIKit
import XCPlayground
import Mapbox

let width: CGFloat = 700
let height: CGFloat = 800

class Responder: NSObject {
    var mapView: MGLMapView?
    func togglePitch(sender: UISwitch)  {
        let camera = mapView!.camera
        camera.pitch = sender.on ? 60 : 0
        mapView!.setCamera(camera, animated: false)
    }
}

//: A control panel
let panelWidth: CGFloat = 200
let panel = UIView(frame: CGRect(x: width - panelWidth, y: 0, width: 200, height: 100))
panel.alpha = 0.8
panel.backgroundColor = UIColor.whiteColor()

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
pitchSwitch.addTarget(responder, action: #selector(responder.togglePitch(_:)), forControlEvents: .ValueChanged)
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
        hidden = hideMarkerSwitchView.on
    }
    
}

//: Define a map delegate

class MapDelegate: NSObject, MGLMapViewDelegate {
    
    var annotationViewByAnnotation = [MGLPointAnnotation: PlaygroundAnnotationView]()
    
    func mapView(mapView: MGLMapView, viewForAnnotation annotation: MGLAnnotation) -> MGLAnnotationView? {
        
        var annotationView = mapView.dequeueReusableAnnotationViewWithIdentifier("annotation") as? PlaygroundAnnotationView
        
        if (annotationView == nil) {
            let av = PlaygroundAnnotationView(reuseIdentifier: "annotation")
            av.frame = CGRect(x: 0, y: 0, width: 30, height: 30)
            av.centerOffset = CGVector(dx: -15, dy: -15)
            av.flat = true
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
    
    func handleTap(press: UILongPressGestureRecognizer) {
        let mapView: MGLMapView = press.view as! MGLMapView
        
        if (press.state == .Recognized) {
            let coordiante: CLLocationCoordinate2D = mapView.convertPoint(press.locationInView(mapView), toCoordinateFromView: mapView)
            let annotation = MGLPointAnnotation()
            annotation.title = "Dropped Marker"
            annotation.coordinate = coordiante
            mapView.addAnnotation(annotation)
            mapView.showAnnotations([annotation], animated: true)
        }
    }
    
}

//: Create a map and its delegate

let lat: CLLocationDegrees = 37.174057
let lng: CLLocationDegrees = -104.490984
let centerCoordinate = CLLocationCoordinate2D(latitude: lat, longitude: lng)

let mapView = MGLMapView(frame: CGRect(x: 0, y: 0, width: width, height: height))
mapView.frame = CGRect(x: 0, y: 0, width: width, height: height)

XCPlaygroundPage.currentPage.liveView = mapView

let mapDelegate = MapDelegate()
mapView.delegate = mapDelegate
responder.mapView = mapView

let tapGesture = UILongPressGestureRecognizer(target: mapDelegate, action: #selector(mapDelegate.handleTap))
mapView.addGestureRecognizer(tapGesture)

//: Zoom in to a location

mapView.setCenterCoordinate(centerCoordinate, zoomLevel: 12, animated: false)

//: Add control panel

mapView.addSubview(panel)
