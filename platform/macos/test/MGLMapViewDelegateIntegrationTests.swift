import XCTest
import Mapbox

class MGLMapViewDelegateIntegrationTests: XCTestCase {

    func testCoverage() {
        MGLSDKTestHelpers.checkTestsContainAllMethods(testClass: MGLMapViewDelegateIntegrationTests.self, in: MGLMapViewDelegate.self)
    }

}

extension MGLMapViewDelegateIntegrationTests: MGLMapViewDelegate {

    func mapView(_ mapView: MGLMapView, shouldChangeFrom oldCamera: MGLMapCamera, to newCamera: MGLMapCamera) -> Bool { return false }

    func mapView(_ mapView: MGLMapView, lineWidthForPolylineAnnotation annotation: MGLPolyline) -> CGFloat { return 0 }

    func mapView(_ mapView: MGLMapView, annotationCanShowCallout annotation: MGLAnnotation) -> Bool { return false }

    func mapView(_ mapView: MGLMapView, imageFor annotation: MGLAnnotation) -> MGLAnnotationImage? { return nil }

    func mapView(_ mapView: MGLMapView, alphaForShapeAnnotation annotation: MGLShape) -> CGFloat { return 0 }

    func mapViewDidFinishRenderingFrame(_ mapView: MGLMapView, fullyRendered: Bool) {}

    func mapViewDidFinishRenderingMap(_ mapView: MGLMapView, fullyRendered: Bool) {}

    func mapViewDidFailLoadingMap(_ mapView: MGLMapView, withError error: Error) {}

    func mapView(_ mapView: MGLMapView, didDeselect annotation: MGLAnnotation) {}

    func mapView(_ mapView: MGLMapView, didSelect annotation: MGLAnnotation) {}

    func mapView(_ mapView: MGLMapView, didFinishLoading style: MGLStyle) {}

    func mapViewWillStartRenderingFrame(_ mapView: MGLMapView) {}

    func mapViewWillStartRenderingMap(_ mapView: MGLMapView) {}

    func mapViewWillStartLoadingMap(_ mapView: MGLMapView) {}

    func mapViewDidFinishLoadingMap(_ mapView: MGLMapView) {}

    func mapViewCameraIsChanging(_ mapView: MGLMapView) {}

    func mapView(_ mapView: MGLMapView, cameraDidChangeAnimated animated: Bool) {}

    func mapView(_ mapView: MGLMapView, cameraWillChangeAnimated animated: Bool) {}

    func mapView(_ mapView: MGLMapView, strokeColorForShapeAnnotation annotation: MGLShape) -> NSColor { return .black }

    func mapView(_ mapView: MGLMapView, fillColorForPolygonAnnotation annotation: MGLPolygon) -> NSColor { return .black }

    func mapView(_ mapView: MGLMapView, calloutViewControllerFor annotation: MGLAnnotation) -> NSViewController? { return nil }

}
