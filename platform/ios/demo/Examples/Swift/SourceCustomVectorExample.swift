import Mapbox

@objc(SourceCustomVectorExample_Swift)

class SourceCustomVectorExample_Swift: UIViewController {
    var mapView: MGLMapView!

    override func viewDidLoad() {
        super.viewDidLoad()

        // Third party vector tile sources can be added.
        
        // In this case we're using custom style JSON (https://www.mapbox.com/mapbox-gl-style-spec/) to add a third party tile source: <https://vector.mapzen.com/osm/all/{z}/{x}/{y}.mvt>
        let customStyleURL = Bundle.main.url(forResource: "third_party_vector_style", withExtension: "json")!

        mapView = MGLMapView(frame: view.bounds, styleURL: customStyleURL)

        mapView.autoresizingMask = [.flexibleWidth, .flexibleHeight]
        mapView.tintColor = .white
        
        view.addSubview(mapView)
    }
}

