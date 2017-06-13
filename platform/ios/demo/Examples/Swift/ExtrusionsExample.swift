//
//  ExtrusionsExample.swift
//  Examples
//
//  Created by Jordan Kiley on 5/10/17.
//  Copyright © 2017 Mapbox. All rights reserved.
//

import Mapbox

@objc(ExtrusionsExample_Swift)

class ExtrusionsExample: UIViewController, MGLMapViewDelegate {
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        let mapView = MGLMapView(frame: view.bounds, styleURL: MGLStyle.lightStyleURL(withVersion: 9))
        
    // Center the map view on the Colosseum in Rome, Italy and set the camera's pitch and distance.
        mapView.camera = MGLMapCamera(lookingAtCenter: CLLocationCoordinate2D(latitude: 41.8902, longitude: 12.4922), fromDistance: 600, pitch: 60, heading: 0)
        mapView.delegate = self
        self.navigationController?.isNavigationBarHidden = true
        
        view.addSubview(mapView)
    }
    
    func mapView(_ mapView: MGLMapView, didFinishLoading style: MGLStyle) {
        
        let array = [1, 2, 4, 6]
        // Access the Mapbox Streets source and use it to create a `MGLFillExtrusionStyleLayer`. The source identifier is `composite`. Use the `sources` property on a style to verify source identifiers.
        if let source = style.source(withIdentifier: "composite") {
            let layer = MGLFillExtrusionStyleLayer(identifier: "buildings", source: source)
            layer.sourceLayerIdentifier = "building"
            
            // Filter out buildings that should not extrude.
            layer.predicate = NSPredicate(format: "extrude == 'true' AND height >= 0")
            
            // Set the fill extrusion height to the value for the building height attribute.
            layer.fillExtrusionHeight = MGLStyleValue(interpolationMode: .identity, sourceStops: nil, attributeName: "height", options: nil)
            layer.fillExtrusionBase = MGLStyleValue(interpolationMode: .identity, sourceStops: nil, attributeName: "min_height", options: nil)
            layer.fillExtrusionOpacity = MGLStyleValue(rawValue: 0.75)
            layer.fillExtrusionColor = MGLStyleValue(rawValue: .white)
            
            // Insert the fill extrusion layer below a POI label layer. If you aren’t sure what the layer is called, you can view the style in Mapbox Studio or iterate over the style’s layers property, printing out each layer’s identifier.
            if let symbolLayer = style.layer(withIdentifier: "poi-scalerank3") {
                style.insertLayer(layer, below: symbolLayer)
            } else {
                style.addLayer(layer)
            }
        }
    }
}
