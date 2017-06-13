import Mapbox

@objc(UserTrackingModesExample_Swift)

class UserTrackingModesExample_Swift: UIViewController, MGLMapViewDelegate {
    var mapView: MGLMapView!
    @IBOutlet var button: UserLocationButton!
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        mapView = MGLMapView(frame: view.bounds, styleURL: MGLStyle.darkStyleURL(withVersion: 9))
        mapView.autoresizingMask = [.flexibleWidth, .flexibleHeight]
        mapView.delegate = self
        
        mapView.tintColor = .red
        mapView.attributionButton.tintColor = .lightGray
        
        view.addSubview(mapView)
        
        setupLocationButton()
    }
    
    func mapView(_ mapView: MGLMapView, didChange mode: MGLUserTrackingMode, animated: Bool) {
        button.updateArrow(for: mode)
    }
    
    @IBAction func locationButtonTapped() {
        var mode: MGLUserTrackingMode
        
        switch (mapView.userTrackingMode) {
        case .none:
            mode = .follow
            break
        case .follow:
            mode = .followWithHeading
            break
        case .followWithHeading:
            mode = .followWithCourse
            break
        case .followWithCourse:
            mode = .none
            break
        }
        
        mapView.userTrackingMode = mode
    }
    
    func setupLocationButton() {
        button = UserLocationButton()
        button.addTarget(self, action: #selector(locationButtonTapped), for: .touchUpInside)
        button.tintColor = mapView.tintColor
        view.addSubview(button)
        
        // Do some basic auto layout.
        button.translatesAutoresizingMaskIntoConstraints = false
        
        let constraints = [
            NSLayoutConstraint(item: button, attribute: .top, relatedBy: .greaterThanOrEqual, toItem: topLayoutGuide, attribute: .bottom, multiplier: 1, constant: 10),
            NSLayoutConstraint(item: button, attribute: .leading, relatedBy: .equal, toItem: view, attribute: .leading, multiplier: 1, constant: 10),
            NSLayoutConstraint(item: button, attribute: .height, relatedBy: .equal, toItem: nil, attribute: .notAnAttribute, multiplier: 1, constant: button.frame.size.height),
            NSLayoutConstraint(item: button, attribute: .width, relatedBy: .equal, toItem: nil, attribute: .notAnAttribute, multiplier: 1, constant: button.frame.size.width)
        ]
        
        view.addConstraints(constraints)
    }
    
}

class UserLocationButton : UIButton {
    private let size: CGFloat = 80
    private var arrow: CAShapeLayer?
    
    required init() {
        super.init(frame: CGRect(x: 0, y: 0, width: size, height: size))
    }
    
    required init?(coder aDecoder: NSCoder) {
        fatalError("init(coder:) has not been implemented")
    }
    
    override func layoutSubviews() {
        backgroundColor = UIColor.black.withAlphaComponent(0.5)
        layer.cornerRadius = 4
        
        layoutArrow()
    }
    
    private func layoutArrow() {
        if arrow == nil {
            let arrow = CAShapeLayer()
            arrow.path = arrowPath()
            arrow.bounds = CGRect(x: 0, y: 0, width: size / 2, height: size / 2)
            arrow.position = CGPoint(x: size / 2, y: size / 2)
            arrow.shouldRasterize = true
            arrow.rasterizationScale = UIScreen.main.scale
            arrow.drawsAsynchronously = true
            
            self.arrow = arrow
            updateArrow(for: .none)
            layer.addSublayer(self.arrow!)
        }
    }
    
    private func arrowPath() -> CGPath {
        let max: CGFloat = size / 2
        
        let bezierPath = UIBezierPath()
        bezierPath.move(to: CGPoint(x: max * 0.5, y: 0))
        bezierPath.addLine(to: CGPoint(x: max * 0.1, y: max))
        bezierPath.addLine(to: CGPoint(x: max * 0.5, y: max * 0.65))
        bezierPath.addLine(to: CGPoint(x: max * 0.9, y: max))
        bezierPath.addLine(to: CGPoint(x: max * 0.5, y: 0))
        bezierPath.close()
        
        return bezierPath.cgPath
    }
    
    func updateArrow(for mode: MGLUserTrackingMode) {
        var stroke: CGColor
        switch (mode) {
        case .none:
            stroke = UIColor.white.cgColor
        case .follow:
            stroke = tintColor.cgColor
        case .followWithHeading, .followWithCourse:
            stroke = UIColor.clear.cgColor
        }
        arrow!.strokeColor = stroke
        
        // Re-center the arrow, based on its current orientation.
        arrow!.position = (mode == .none || mode == .followWithCourse) ? CGPoint(x: size / 2, y: size / 2) : CGPoint(x: size / 2 + 2, y: size / 2 - 2)
        
        arrow!.fillColor = (mode == .none || mode == .follow) ? UIColor.clear.cgColor : tintColor.cgColor
        
        let rotation: CGFloat = (mode == .follow || mode == .followWithHeading) ? 0.66 : 0
        arrow!.setAffineTransform(CGAffineTransform.identity.rotated(by: rotation))
        
        layoutIfNeeded()
    }
}
