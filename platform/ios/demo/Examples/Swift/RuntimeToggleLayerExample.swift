import Mapbox

@objc(RuntimeToggleLayerExample_Swift)

class RuntimeToggleLayerExample_Swift: UIViewController, MGLMapViewDelegate {
    var mapView: MGLMapView!
    var contoursLayer: MGLStyleLayer?
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        mapView = MGLMapView(frame: view.bounds)
        mapView.autoresizingMask = [.flexibleWidth, .flexibleHeight]
        
        mapView.setCenter(CLLocationCoordinate2D(latitude: 37.745395, longitude: -119.594421), zoomLevel: 11, animated: false)
        view.addSubview(mapView)
        
        addToggleButton()
        
        mapView.delegate = self
    }
    
    // Wait until the style is loaded before modifying the map style
    func mapView(_ mapView: MGLMapView, didFinishLoading style: MGLStyle) {
        addLayer(to: style)
    }
    
    func addLayer(to style: MGLStyle) {
        let source = MGLVectorSource(identifier: "contours", configurationURL: NSURL(string: "mapbox://mapbox.mapbox-terrain-v2")! as URL)
        
        let layer = MGLLineStyleLayer(identifier: "contours", source: source)
        layer.sourceLayerIdentifier = "contour"
        layer.lineJoin = MGLStyleValue(rawValue: NSValue(mglLineJoin: .round))
        layer.lineCap = MGLStyleValue(rawValue: NSValue(mglLineCap: .round))
        layer.lineColor = MGLStyleValue(rawValue: UIColor.brown)
        layer.lineWidth = MGLStyleValue(rawValue: 1.0)
        
        style.addSource(source)
        if let water = style.layer(withIdentifier: "water") {
            // You can insert a layer below an existing style layer
            style.insertLayer(layer, below: water)
        } else {
            // or you can simply add it above all layers
            style.addLayer(layer)
        }
        
        self.contoursLayer = layer
        
        showContours()
    }
    
    func toggleLayer(sender: UIButton) {
        sender.isSelected = !sender.isSelected
        if sender.isSelected {
            showContours()
        } else {
            hideContours()
        }
    }
    
    func showContours() {
        self.contoursLayer?.isVisible = true
    }
    
    func hideContours() {
        self.contoursLayer?.isVisible = false
    }
    
    func addToggleButton() {
        let button = UIButton(type: .system)
        button.autoresizingMask = [.flexibleTopMargin, .flexibleLeftMargin, .flexibleRightMargin]
        button.setTitle("Toggle Contours", for: .normal)
        button.isSelected = true
        button.sizeToFit()
        button.center.x = self.view.center.x
        button.frame = CGRect(origin: CGPoint(x: button.frame.origin.x, y: self.view.frame.size.height - button.frame.size.height - 5), size: button.frame.size)
        button.addTarget(self, action: #selector(RuntimeToggleLayerExample_Swift.toggleLayer(sender:)), for: .touchUpInside)
        self.view.addSubview(button)
    }
}
