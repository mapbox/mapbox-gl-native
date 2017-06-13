import Mapbox

@objc(CustomRasterStyleExample_Swift)

class CustomRasterStyleExample_Swift: UIViewController {
    override func viewDidLoad() {
        super.viewDidLoad()
        
        let styleURL = URL(string: "https://www.mapbox.com/ios-sdk/files/mapbox-raster-v8.json")
        // Local paths are also acceptable.
        
        let mapView = MGLMapView(frame: view.bounds, styleURL: styleURL)
        mapView.autoresizingMask = [.flexibleWidth, .flexibleHeight]
        
        view.addSubview(mapView)
    }
}
