import XCTest

class MGLMapSnapshotterSwiftTests: MGLMapViewIntegrationTest {

    // Create snapshot options
    private class func snapshotterOptions(size: CGSize) -> MGLMapSnapshotOptions {
        let camera = MGLMapCamera()

        let options = MGLMapSnapshotOptions(styleURL: MGLStyle.satelliteStreetsStyleURL, camera: camera, size: size)

        let sw = CLLocationCoordinate2D(latitude: 52.3, longitude: 13.0)
        let ne = CLLocationCoordinate2D(latitude: 52.5, longitude: 13.2)
        options.coordinateBounds = MGLCoordinateBounds(sw:sw, ne:ne)

        return options
    }

    func testCapturingSnapshotterInSnapshotCompletion() {
        // See the Obj-C testDeallocatingSnapshotterDuringSnapshot
        // This Swift test, is essentially the same except for capturing the snapshotter
        guard validAccessToken() != nil else {
            return
        }

        let timeout: TimeInterval = 5.0
        let expectation = self.expectation(description: "snapshot")

        let options = MGLMapSnapshotterSwiftTests.snapshotterOptions(size: mapView.bounds.size)

        let backgroundQueue = DispatchQueue.main

        backgroundQueue.async {
            let dg = DispatchGroup()
            dg.enter()

            DispatchQueue.main.async {

                let snapshotter = MGLMapSnapshotter(options: options)

                snapshotter.start(completionHandler: { (snapshot, error) in

//                    // Without capturing snapshotter:
//                    XCTAssertNil(snapshot)
//                    XCTAssertNotNil(error)

                    // Capture snapshotter
                    dump(snapshotter)
                    XCTAssertNotNil(snapshot)
                    XCTAssertNil(error)

                    dg.leave()
                })
            }

            dg.notify(queue: .main) {
                expectation.fulfill()
            }
        }

        wait(for: [expectation], timeout: timeout)
    }
}
