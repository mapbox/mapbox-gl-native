import Mapbox

@objc(SourceCustomRasterExample_Swift)

class SourceCustomRasterExample_Swift: UIViewController, MGLMapViewDelegate {
    var mapView: MGLMapView!
    var rasterLayer: MGLRasterStyleLayer?

    override func viewDidLoad() {
        super.viewDidLoad()

        mapView = MGLMapView(frame: view.bounds)
        mapView.autoresizingMask = [.flexibleWidth, .flexibleHeight]

        mapView.setCenter(CLLocationCoordinate2D(latitude: 45.5188, longitude: -122.6748), zoomLevel: 13, animated: false)

        mapView.delegate = self

        view.addSubview(mapView)

        // Add a UISlider that will control the raster layer’s opacity.
        addSlider()
    }

    func mapView(_ mapView: MGLMapView, didFinishLoading style: MGLStyle) {
        // Add a new raster source and layer.
        let source = MGLRasterSource(identifier: "stamen-watercolor", tileURLTemplates: ["https://stamen-tiles.a.ssl.fastly.net/watercolor/{z}/{x}/{y}.jpg"], options: [ .tileSize: 256 ])
        let rasterLayer = MGLRasterStyleLayer(identifier: "stamen-watercolor", source: source)

        style.addSource(source)
        style.addLayer(rasterLayer)

        self.rasterLayer = rasterLayer
    }
    
    func updateLayerOpacity(_ sender: UISlider) {
        rasterLayer?.rasterOpacity = MGLStyleValue(rawValue: NSNumber(value: sender.value))
    }

    func addSlider() {
        let padding: CGFloat = 10
        let slider = UISlider(frame: CGRect(x: padding, y: self.view.frame.size.height - 44 - 30, width: self.view.frame.size.width - padding *  2, height: 44))
        slider.autoresizingMask = [.flexibleTopMargin, .flexibleLeftMargin, .flexibleRightMargin]
        slider.minimumValue = 0
        slider.maximumValue = 1
        slider.value = 1
        slider.addTarget(self, action: #selector(updateLayerOpacity), for: .valueChanged)
        view.addSubview(slider)
    }
}
