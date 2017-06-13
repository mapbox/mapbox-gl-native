import Mapbox

@objc(RuntimeMultipleAnnotationsExample_Swift)

class RuntimeMultipleAnnotationsExample_Swift: UIViewController, MGLMapViewDelegate {
    var mapView: MGLMapView!

    override func viewDidLoad() {
        super.viewDidLoad()

        let mapView = MGLMapView(frame: view.bounds)
        mapView.autoresizingMask = [.flexibleWidth, .flexibleHeight]

        mapView.setCenter(CLLocationCoordinate2D(latitude: 37.090240, longitude: -95.712891), zoomLevel: 2, animated: false)

        mapView.delegate = self

        view.addSubview(mapView)

        // Add our own gesture recognizer to handle taps on our custom map features.
        mapView.addGestureRecognizer(UITapGestureRecognizer(target: self, action: #selector(handleMapTap(sender:))))
        
        self.mapView = mapView
    }

    func mapView(_ mapView: MGLMapView, didFinishLoading style: MGLStyle) {
        fetchPoints() { [weak self] (features) in
            self?.addItemsToMap(features: features)
        }
    }

    func addItemsToMap(features: [MGLPointFeature]) {
        // MGLMapView.style is optional, so you must guard against it not being set.
        guard let style = mapView.style else { return }

        // You can add custom UIImages to the map style.
        // These can be referenced by an MGLSymbolStyleLayer’s iconImage property.
        style.setImage(UIImage(named: "lighthouse")!, forName: "lighthouse")

        // Add the features to the map as a shape source.
        let source = MGLShapeSource(identifier: "lighthouses", features: features, options: nil)
        style.addSource(source)

        let lighthouseColor = UIColor(red: 0.08, green: 0.44, blue: 0.96, alpha: 1.0)

        // Use MGLCircleStyleLayer to represent the points with simple circles.
        // In this case, we can use style functions to gradually change properties between zoom level 2 and 7: the circle opacity from 50% to 100% and the circle radius from 2pt to 3pt.
        let circles = MGLCircleStyleLayer(identifier: "lighthouse-circles", source: source)
        circles.circleColor = MGLStyleValue(rawValue: lighthouseColor)
        circles.circleOpacity = MGLStyleValue(interpolationMode: .exponential,
            cameraStops: [2: MGLStyleValue(rawValue: 0.5),
                          7: MGLStyleValue(rawValue: 1)],
            options: nil)
        circles.circleRadius = MGLStyleValue(interpolationMode: .exponential,
            cameraStops: [2: MGLStyleValue(rawValue: 2),
                          7: MGLStyleValue(rawValue: 3)],
            options: nil)

        // Use MGLSymbolStyleLayer for more complex styling of points including custom icons and text rendering.
        let symbols = MGLSymbolStyleLayer(identifier: "lighthouse-symbols", source: source)
        symbols.iconImageName = MGLStyleValue(rawValue: "lighthouse")
        symbols.iconColor = MGLStyleValue(rawValue: lighthouseColor)
        symbols.iconScale = MGLStyleValue(rawValue: 0.5)
        symbols.iconOpacity = MGLStyleValue(interpolationMode: .exponential,
            cameraStops: [5.9: MGLStyleValue(rawValue: 0),
                          6: MGLStyleValue(rawValue: 1)],
            options: nil)
        symbols.iconHaloColor = MGLStyleValue(rawValue: UIColor.white.withAlphaComponent(0.5))
        symbols.iconHaloWidth = MGLStyleValue(rawValue: 1)
        // {name} references the "name" key in an MGLPointFeature’s attributes dictionary.
        symbols.text = MGLStyleValue(rawValue: "{name}")
        symbols.textColor = symbols.iconColor
        symbols.textFontSize = MGLStyleValue(interpolationMode: .exponential,
            cameraStops: [10: MGLStyleValue(rawValue: 10),
                          16: MGLStyleValue(rawValue: 16)],
            options: nil)
        symbols.textTranslation = MGLStyleValue(rawValue: NSValue(cgVector: CGVector(dx: 10, dy: 0)))
        symbols.textOpacity = symbols.iconOpacity
        symbols.textHaloColor = symbols.iconHaloColor
        symbols.textHaloWidth = symbols.iconHaloWidth
        symbols.textJustification = MGLStyleValue(rawValue: NSValue(mglTextJustification: .left))
        symbols.textAnchor = MGLStyleValue(rawValue: NSValue(mglTextAnchor: .left))

        style.addLayer(circles)
        style.addLayer(symbols)
    }

    // MARK: - Feature interaction
    func handleMapTap(sender: UITapGestureRecognizer) {
        if sender.state == .ended {
            // Limit feature selection to just the following layer identifiers.
            let layerIdentifiers: Set = ["lighthouse-symbols", "lighthouse-circles"]

            // Try matching the exact point first.
            let point = sender.location(in: sender.view!)
            for f in mapView.visibleFeatures(at: point, styleLayerIdentifiers:layerIdentifiers)
              where f is MGLPointFeature {
                showCallout(feature: f as! MGLPointFeature)
                return
            }

            let touchCoordinate = mapView.convert(point, toCoordinateFrom: sender.view!)
            let touchLocation = CLLocation(latitude: touchCoordinate.latitude, longitude: touchCoordinate.longitude)

            // Otherwise, get all features within a rect the size of a touch (44x44).
            let touchRect = CGRect(origin: point, size: .zero).insetBy(dx: -22.0, dy: -22.0)
            let possibleFeatures = mapView.visibleFeatures(in: touchRect, styleLayerIdentifiers: Set(layerIdentifiers)).filter { $0 is MGLPointFeature }

            // Select the closest feature to the touch center.
            let closestFeatures = possibleFeatures.sorted(by: {
                return CLLocation(latitude: $0.coordinate.latitude, longitude: $0.coordinate.longitude).distance(from: touchLocation) < CLLocation(latitude: $1.coordinate.latitude, longitude: $1.coordinate.longitude).distance(from: touchLocation)
            })
            if let f = closestFeatures.first {
                showCallout(feature: f as! MGLPointFeature)
                return
            }
            
            // If no features were found, deselect the selected annotation, if any.
            mapView.deselectAnnotation(mapView.selectedAnnotations.first, animated: true)
        }
    }

    func showCallout(feature: MGLPointFeature) {
        let point = MGLPointFeature()
        point.title = feature.attributes["name"] as? String
        point.coordinate = feature.coordinate

        // Selecting an feature that doesn’t already exist on the map will add a new annotation view.
        // We’ll need to use the map’s delegate methods to add an empty annotation view and remove it when we’re done selecting it.
        mapView.selectAnnotation(point, animated: true)
    }

    // MARK: - MGLMapViewDelegate

    func mapView(_ mapView: MGLMapView, annotationCanShowCallout annotation: MGLAnnotation) -> Bool {
        return true
    }

    func mapView(_ mapView: MGLMapView, didDeselect annotation: MGLAnnotation) {
        mapView.removeAnnotations([annotation])
    }

    func mapView(_ mapView: MGLMapView, viewFor annotation: MGLAnnotation) -> MGLAnnotationView? {
        // Create an empty view annotation. Set a frame to offset the callout.
        return MGLAnnotationView(frame: CGRect(x: 0, y: 0, width: 20, height: 20))
    }

    // MARK: - Data fetching and parsing

    func fetchPoints(withCompletion completion: @escaping (([MGLPointFeature]) -> Void)) {
        // Wikidata query for all lighthouses in the United States: http://tinyurl.com/zrl2jc4
        let query = "SELECT DISTINCT ?item " +
            "?itemLabel ?coor ?image " +
            "WHERE " +
            "{ " +
            "?item wdt:P31 wd:Q39715 . " +
            "?item wdt:P17 wd:Q30 . " +
            "?item wdt:P625 ?coor . " +
            "OPTIONAL { ?item wdt:P18 ?image } . " +
            "SERVICE wikibase:label { bd:serviceParam wikibase:language \"en\" } " +
            "} " +
        "ORDER BY ?itemLabel"

        let characterSet = NSMutableCharacterSet()
        characterSet.formUnion(with: CharacterSet.urlQueryAllowed)
        characterSet.removeCharacters(in: "?")
        characterSet.removeCharacters(in: "&")
        characterSet.removeCharacters(in: ":")

        let encodedQuery = query.addingPercentEncoding(withAllowedCharacters: characterSet as CharacterSet)!

        let request = URLRequest(url: URL(string: "https://query.wikidata.org/sparql?query=\(encodedQuery)&format=json")!)

        URLSession.shared.dataTask(with: request, completionHandler: { (data, response, error) in
            guard let data = data else { return }
            guard let json = try? JSONSerialization.jsonObject(with: data, options:[]) as? [String: AnyObject] else { return }
            guard let results = json?["results"] as? [String: AnyObject] else { return }
            guard let items = results["bindings"] as? [[String: AnyObject]] else { return }
            DispatchQueue.main.async {
            completion(self.parseJSONItems(items: items))
            }
        }).resume()
    }

    func parseJSONItems(items: [[String: AnyObject]]) -> [MGLPointFeature] {
        var features = [MGLPointFeature]()
        for item in items {
            guard let label = item["itemLabel"] as? [String: AnyObject],
            let title = label["value"] as? String else { continue }
            guard let coor = item["coor"] as? [String: AnyObject],
            let point = coor["value"] as? String else { continue }

            let parsedPoint = point.replacingOccurrences(of: "Point(", with: "").replacingOccurrences(of: ")", with: "")
            let pointComponents = parsedPoint.components(separatedBy: " ")
            let coordinate = CLLocationCoordinate2D(latitude: Double(pointComponents[1])!, longitude: Double(pointComponents[0])!)
            let feature = MGLPointFeature()
            feature.coordinate = coordinate
            feature.title = title
            // A feature’s attributes can used by runtime styling for things like text labels.
            feature.attributes = [
                "name": title
            ]
            features.append(feature)
        }
        return features
    }
}
