import XCTest

class MGLSourceTests: MGLMapViewIntegrationTest {

    // See testForRaisingExceptionsOnStaleStyleObjects for Obj-C sibling.
    func testForRaisingExceptionsOnStaleStyleObjectsOnRemoveFromMapView() {
        
        guard
            let configURL = URL(string: "mapbox://examples.2uf7qges") else {
                XCTFail()
                return
        }
        
        let source = MGLVectorTileSource(identifier: "trees", configurationURL: configURL)
        mapView.style?.addSource(source)
        
        let bundle = Bundle(for: type(of: self))
        
        guard let styleURL = bundle.url(forResource: "one-liner", withExtension: "json") else {
            XCTFail()
            return
        }
        
        styleLoadingExpectation = nil;

        mapView.centerCoordinate = CLLocationCoordinate2D(latitude : 38.897, longitude : -77.039)
        mapView.zoomLevel        = 10.5
        mapView.styleURL         = styleURL
        
        waitForMapViewToFinishLoadingStyle(withTimeout: 10.0)
        
        let expect = expectation(description: "Remove source should error")
        
        do {
            try mapView.style?.removeSource(source, error: ())
        }
        catch let error as NSError {
            XCTAssertEqual(error.domain, MGLErrorDomain)
            XCTAssertEqual(error.code, MGLErrorCode.sourceCannotBeRemovedFromStyle.rawValue)
            expect.fulfill()
        }
        
        wait(for: [expect], timeout: 0.1)
    }
}
