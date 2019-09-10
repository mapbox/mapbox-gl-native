// From https://github.com/mapbox/mapbox-gl-native/issues/15342#issue-478458025
import Foundation
import Mapbox

class TestMapController: UIViewController, MGLMapViewDelegate {
    
    var mapView: MGLMapView!
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        mapView = MGLMapView(frame: view.bounds, styleURL: MGLStyle.lightStyleURL)
        mapView.autoresizingMask = [.flexibleWidth, .flexibleHeight]
        mapView.tintColor = .darkGray
        mapView.delegate = self
        view.addSubview(mapView)
        
    }
    
    func mapView(_ mapView: MGLMapView, didFinishLoading style: MGLStyle) {
        let source = MGLShapeSource(identifier: "pictures", features: [], options: nil)
        let blueBoxImage = TestMapController.singleColoredImage(of: .blue, and: CGSize(width: 50, height: 50))
        style.setImage(blueBoxImage, forName: "picture")
        let layer = TestMapController.getLayer(for: source)
        style.addSource(source)
        style.addLayer(layer)
    }
    
    static func singleColoredImage(of color: UIColor, and size: CGSize) -> UIImage {
        let format = UIGraphicsImageRendererFormat()
        return UIGraphicsImageRenderer(size: size, format: format).image { (context) in
            color.setFill()
            context.fill(CGRect(origin: .zero, size: size))
        }
    }
    
    static func getLayer(for source: MGLShapeSource) -> MGLStyleLayer {
        let layer = MGLSymbolStyleLayer(identifier: "pictures", source: source)
        layer.iconAllowsOverlap = NSExpression(forConstantValue: true)
        layer.minimumZoomLevel = 6.0
        let nameOfPictures = "picture"
        let matching = "'\(nameOfPictures)', '\(nameOfPictures)'"
        let formatImageName = "MGL_MATCH(highlight, \(matching), '\(nameOfPictures)')"
        let functionImage = NSExpression(format: formatImageName)
        layer.iconImageName = functionImage
        layer.keepsIconUpright = NSExpression(forConstantValue: 1)
        let formatScale = "mgl_interpolate:withCurveType:parameters:stops:($zoomLevel, 'exponential', 1, %@)"
        layer.iconScale = NSExpression(format: formatScale, [6: 0.3, 11: 0.6, 14: 0.88])
        return layer
    }
}
