import XCTest
import Mapbox

class CustomAnnotationView: MGLAnnotationView {
    
    override init(reuseIdentifier: String?) {
        super.init(reuseIdentifier: reuseIdentifier)
    }
    
    required init?(coder aDecoder: NSCoder) {
        super.init(coder: aDecoder)
    }
    
}

class MGLAnnotationViewIntegrationTests: XCTestCase {
    
    func testCreatingCustomAnnotationView() {
        let customAnnotationView = CustomAnnotationView(reuseIdentifier: "resuse-id")
        XCTAssertNotNil(customAnnotationView)
    }
    
}
