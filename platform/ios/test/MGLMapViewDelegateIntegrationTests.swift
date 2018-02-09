import XCTest
import Mapbox

class MGLMapViewDelegateIntegrationTests: XCTestCase {

    func testCoverage() {
        MGLSDKTestHelpers.checkTestsContainAllMethods(testClass: MGLMapViewDelegateIntegrationTests.self, in: MGLMapViewDelegate.self)
    }

}

extension MGLMapViewDelegateIntegrationTests: MGLMapViewDelegate {

    func mapViewRegionIsChanging(_ mapView: MGLMapView) {}

    func mapViewRegionIsChanging(_ mapView: MGLMapView, reason: MGLCameraChangeReason) {}

    func mapView(_ mapView: MGLMapView, regionIsChangingWith reason: MGLCameraChangeReason) {}

    func mapView(_ mapView: MGLMapView, didChange mode: MGLUserTrackingMode, animated: Bool) {}

    func mapViewDidFinishLoadingMap(_ mapView: MGLMapView) {}

    func mapViewDidStopLocatingUser(_ mapView: MGLMapView) {}

    func mapViewWillStartLoadingMap(_ mapView: MGLMapView) {}

    func mapViewWillStartLocatingUser(_ mapView: MGLMapView) {}

    func mapViewWillStartRenderingMap(_ mapView: MGLMapView) {}

    func mapViewWillStartRenderingFrame(_ mapView: MGLMapView) {}

    func mapView(_ mapView: MGLMapView, didFinishLoading style: MGLStyle) {}

    func mapView(_ mapView: MGLMapView, didSelect annotation: MGLAnnotation) {}

    func mapView(_ mapView: MGLMapView, didDeselect annotation: MGLAnnotation) {}

    func mapView(_ mapView: MGLMapView, didSingleTapAt coordinate: CLLocationCoordinate2D) {}

    func mapView(_ mapView: MGLMapView, regionDidChangeAnimated animated: Bool) {}

    func mapView(_ mapView: MGLMapView, regionDidChangeWith reason: MGLCameraChangeReason, animated: Bool) {}

    func mapView(_ mapView: MGLMapView, regionWillChangeAnimated animated: Bool) {}

    func mapView(_ mapView: MGLMapView, regionWillChangeWith reason: MGLCameraChangeReason, animated: Bool) {}

    func mapViewDidFailLoadingMap(_ mapView: MGLMapView, withError error: Error) {}

    func mapView(_ mapView: MGLMapView, didUpdate userLocation: MGLUserLocation?) {}

    func mapViewDidFinishRenderingMap(_ mapView: MGLMapView, fullyRendered: Bool) {}

    func mapView(_ mapView: MGLMapView, didFailToLocateUserWithError error: Error) {}

    func mapView(_ mapView: MGLMapView, tapOnCalloutFor annotation: MGLAnnotation) {}

    func mapViewDidFinishRenderingFrame(_ mapView: MGLMapView, fullyRendered: Bool) {}

    func mapView(_ mapView: MGLMapView, didAdd annotationViews: [MGLAnnotationView]) {}

    func mapView(_ mapView: MGLMapView, didSelect annotationView: MGLAnnotationView) {}

    func mapView(_ mapView: MGLMapView, didDeselect annotationView: MGLAnnotationView) {}

    func mapView(_ mapView: MGLMapView, alphaForShapeAnnotation annotation: MGLShape) -> CGFloat { return 0 }

    func mapView(_ mapView: MGLMapView, viewFor annotation: MGLAnnotation) -> MGLAnnotationView? { return nil }

    func mapView(_ mapView: MGLMapView, imageFor annotation: MGLAnnotation) -> MGLAnnotationImage? { return nil }

    func mapView(_ mapView: MGLMapView, annotationCanShowCallout annotation: MGLAnnotation) -> Bool { return false }

    func mapView(_ mapView: MGLMapView, calloutViewFor annotation: MGLAnnotation) -> MGLCalloutView? { return nil }

    func mapView(_ mapView: MGLMapView, strokeColorForShapeAnnotation annotation: MGLShape) -> UIColor { return .black }

    func mapView(_ mapView: MGLMapView, fillColorForPolygonAnnotation annotation: MGLPolygon) -> UIColor { return .black }

    func mapView(_ mapView: MGLMapView, leftCalloutAccessoryViewFor annotation: MGLAnnotation) -> UIView? { return nil }

    func mapView(_ mapView: MGLMapView, lineWidthForPolylineAnnotation annotation: MGLPolyline) -> CGFloat { return 0 }

    func mapView(_ mapView: MGLMapView, rightCalloutAccessoryViewFor annotation: MGLAnnotation) -> UIView? { return nil }

    func mapView(_ mapView: MGLMapView, annotation: MGLAnnotation, calloutAccessoryControlTapped control: UIControl) {}

    func mapView(_ mapView: MGLMapView, shouldChangeFrom oldCamera: MGLMapCamera, to newCamera: MGLMapCamera) -> Bool { return false }

    func mapView(_ mapView: MGLMapView, shouldChangeFrom oldCamera: MGLMapCamera, to newCamera: MGLMapCamera, reason: MGLCameraChangeReason) -> Bool { return false }
}
