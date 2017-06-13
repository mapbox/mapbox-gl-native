
import Mapbox

@objc(DDSLayerSelectionExample_Swift)

class DDSLayerSelectionExample_Swift: UIViewController, MGLMapViewDelegate, UIGestureRecognizerDelegate {
    
    var mapView : MGLMapView!
 
    override func viewDidLoad() {
        super.viewDidLoad()
        
        mapView = MGLMapView(frame: view.bounds)
        mapView.delegate = self
        mapView.setCenter(CLLocationCoordinate2D(latitude:39.23225, longitude:-97.91015), animated: false)
        mapView.autoresizingMask = [.flexibleHeight, .flexibleWidth]
        view.addSubview(mapView)
        
        // Add a tap gesture recognizer to the map view.
        let gesture = UITapGestureRecognizer(target: self, action: #selector(handleTap(_:)))
        gesture.delegate = self
        mapView.addGestureRecognizer(gesture)
    }
    
    func mapView(_ mapView: MGLMapView, didFinishLoading style: MGLStyle) {
        
        // Load a tileset containing U.S. states and their population density. For more information about working with tilesets, see: https://www.mapbox.com/help/studio-manual-tilesets/
        let url = URL(string: "mapbox://examples.69ytlgls")!
        let source = MGLVectorSource(identifier: "state-source", configurationURL: url)
        style.addSource(source)
        
        let layer = MGLFillStyleLayer(identifier: "state-layer", source: source)
        
        // Access the tileset layer.
        layer.sourceLayerIdentifier = "stateData_2-dx853g"
        
        // Create a stops dictionary. This defines the relationship between population density and a UIColor.
        let stops = [0: MGLStyleValue(rawValue: UIColor.yellow),
                     600: MGLStyleValue(rawValue: UIColor.red),
                     1200: MGLStyleValue(rawValue: UIColor.blue)]
        
        // Style the fill color using the stops dictionary, exponential interpolation mode, and the feature attribute name.
        layer.fillColor = MGLStyleValue(interpolationMode: .exponential, sourceStops: stops, attributeName: "density", options: [.defaultValue: MGLStyleValue(rawValue: UIColor.white)])
        
        // Insert the new layer below the Mapbox Streets layer that contains state border lines. See the layer reference for more information about layer names: https://www.mapbox.com/vector-tiles/mapbox-streets-v7/
        let symbolLayer = style.layer(withIdentifier: "admin-3-4-boundaries")
        style.insertLayer(layer, below: symbolLayer!)
    }
    
    func handleTap(_ gesture: UITapGestureRecognizer) {
        
        // Get the CGPoint where the user tapped.
        let spot = gesture.location(in: mapView)
        
        // Access the features at that point within the state layer.
        let features = mapView.visibleFeatures(at: spot, styleLayerIdentifiers: Set(["state-layer"]))
        
        // Get the name of the selected state.
        if let feature = features.first, let state = feature.attribute(forKey: "name") as? String{
            changeOpacity(name: state)
        } else {
            changeOpacity(name: "")
        }
    }
    
    func changeOpacity(name: String) {
        let layer = mapView.style?.layer(withIdentifier: "state-layer") as! MGLFillStyleLayer
        
        // Check if a state was selected, then change the opacity of the states that were not selected.
        if name.characters.count > 0 {
            layer.fillOpacity = MGLStyleValue(interpolationMode: .categorical, sourceStops: [name: MGLStyleValue<NSNumber>(rawValue: 1)], attributeName: "name", options: [.defaultValue: MGLStyleValue<NSNumber>(rawValue: 0)])
        } else {
            // Reset the opacity for all states if the user did not tap on a state.
            layer.fillOpacity = MGLStyleValue(rawValue: 1)
        }
    }
}
