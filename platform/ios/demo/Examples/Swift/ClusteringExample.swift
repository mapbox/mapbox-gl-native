import Mapbox

@objc(ClusteringExample_Swift)

class ClusteringExample_Swift: UIViewController, MGLMapViewDelegate {

    var mapView: MGLMapView!
    var icon: UIImage!
    var popup: UILabel?

    override func viewDidLoad() {
        super.viewDidLoad()

        mapView = MGLMapView(frame: view.bounds, styleURL: MGLStyle.lightStyleURL(withVersion: 9))
        mapView.autoresizingMask = [.flexibleWidth, .flexibleHeight]
        mapView.tintColor = .darkGray
        mapView.delegate = self
        view.addSubview(mapView)

        icon = UIImage(named: "port")
    }

    func mapView(_ mapView: MGLMapView, didFinishLoading style: MGLStyle) {
        let url = URL(fileURLWithPath: Bundle.main.path(forResource: "ports", ofType: "geojson")!)

        let source = MGLShapeSource(identifier: "clusteredPorts",
                                    url: url,
                                    options: [.clustered: true, .clusterRadius: icon.size.width])
        style.addSource(source)

        // Use a template image so that we can tint it with the `iconColor` runtime styling property.
        style.setImage(icon.withRenderingMode(.alwaysTemplate), forName: "icon")

        // Show unclustered features as icons. The `cluster` attribute is built into clustering-enabled source features.
        let ports = MGLSymbolStyleLayer(identifier: "ports", source: source)
        ports.iconImageName = MGLStyleValue(rawValue: "icon")
        ports.iconColor = MGLStyleValue(rawValue: UIColor.darkGray.withAlphaComponent(0.9))
        ports.predicate = NSPredicate(format: "%K != YES", "cluster")
        style.addLayer(ports)

        // Color clustered features based on clustered point counts.
        let stops = [
            20:  MGLStyleValue(rawValue: UIColor.lightGray),
            50:  MGLStyleValue(rawValue: UIColor.orange),
            100: MGLStyleValue(rawValue: UIColor.red),
            200: MGLStyleValue(rawValue: UIColor.purple)
        ]

        // Show clustered features as circles. The `point_count` attribute is built into clustering-enabled source features.
        let circlesLayer = MGLCircleStyleLayer(identifier: "clusteredPorts", source: source)
        circlesLayer.circleRadius = MGLStyleValue(rawValue: NSNumber(value: Double(icon.size.width) / 2))
        circlesLayer.circleOpacity = MGLStyleValue(rawValue: 0.75)
        circlesLayer.circleStrokeColor = MGLStyleValue(rawValue: UIColor.white.withAlphaComponent(0.75))
        circlesLayer.circleStrokeWidth = MGLStyleValue(rawValue: 2)
        circlesLayer.circleColor = MGLSourceStyleFunction(interpolationMode: .interval,
                                                          stops: stops,
                                                          attributeName: "point_count",
                                                          options: nil)
        circlesLayer.predicate = NSPredicate(format: "%K == YES", "cluster")
        style.addLayer(circlesLayer)

        // Label cluster circles with a layer of text indicating feature count. Per text token convention, wrap the attribute in {}.
        let numbersLayer = MGLSymbolStyleLayer(identifier: "clusteredPortsNumbers", source: source)
        numbersLayer.textColor = MGLStyleValue(rawValue: UIColor.white)
        numbersLayer.textFontSize = MGLStyleValue(rawValue: NSNumber(value: Double(icon.size.width) / 2))
        numbersLayer.iconAllowsOverlap = MGLStyleValue(rawValue: true)
        numbersLayer.text = MGLStyleValue(rawValue: "{point_count}")
        numbersLayer.predicate = NSPredicate(format: "%K == YES", "cluster")
        style.addLayer(numbersLayer)

        // Add a tap gesture for zooming in to clusters or showing popups on individual features.
        view.addGestureRecognizer(UITapGestureRecognizer(target: self, action: #selector(handleTap(_:))))
    }

    func mapViewRegionIsChanging(_ mapView: MGLMapView) {
        showPopup(false, animated: false)
    }

    func handleTap(_ tap: UITapGestureRecognizer) {
        if tap.state == .ended {
            let point = tap.location(in: tap.view)
            let width = icon.size.width
            let rect = CGRect(x: point.x - width / 2, y: point.y - width / 2, width: width, height: width)

            let clusters = mapView.visibleFeatures(in: rect, styleLayerIdentifiers: ["clusteredPorts"])
            let ports = mapView.visibleFeatures(in: rect, styleLayerIdentifiers: ["ports"])

            if clusters.count > 0 {
                showPopup(false, animated: true)
                let cluster = clusters.first!
                mapView.setCenter(cluster.coordinate, zoomLevel: (mapView.zoomLevel + 1), animated: true)
            } else if ports.count > 0 {
                let port = ports.first!

                if popup == nil {
                    popup = UILabel(frame: CGRect(x: 0, y: 0, width: 100, height: 40))
                    popup!.backgroundColor = UIColor.white.withAlphaComponent(0.9)
                    popup!.layer.cornerRadius = 4
                    popup!.layer.masksToBounds = true
                    popup!.textAlignment = .center
                    popup!.lineBreakMode = .byTruncatingTail
                    popup!.font = UIFont.systemFont(ofSize: 16)
                    popup!.textColor = UIColor.black
                    popup!.alpha = 0
                    view.addSubview(popup!)
                }

                popup!.text = (port.attribute(forKey: "name")! as! String)
                let size = (popup!.text! as NSString).size(attributes: [NSFontAttributeName: popup!.font])
                popup!.bounds = CGRect(x: 0, y: 0, width: size.width, height: size.height).insetBy(dx: -10, dy: -10)
                let point = mapView.convert(port.coordinate, toPointTo: mapView)
                popup!.center = CGPoint(x: point.x, y: point.y - 50)

                if popup!.alpha < 1 {
                    showPopup(true, animated: true)
                }
            } else {
                showPopup(false, animated: true)
            }
        }
    }

    func showPopup(_ shouldShow: Bool, animated: Bool) {
        let alpha: CGFloat = (shouldShow ? 1 : 0)
        if animated {
            UIView.animate(withDuration: 0.25) { [unowned self] in
                self.popup?.alpha = alpha
            }
        } else {
            popup?.alpha = alpha
        }
    }

}
