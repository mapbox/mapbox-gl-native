import XCTest

class MGLSDKTestHelpers {

    class func checkTestsContainAllMethods(testClass: Swift.AnyClass, in p: Protocol) {
        let testMethods = self.classMethodDescriptions(testClass)
        let subjectMethods = self.protocolMethodDescriptions(p)

        for method in subjectMethods {
            if !testMethods.contains(method) {
                XCTFail("\(String(describing: testClass)) does not contain \(method) from \(String(describing: p))")
            }
        }

        XCTAssert(true)
    }

}

extension MGLSDKTestHelpers {

    class func protocolMethodDescriptions(_ p: Protocol) -> Set<String> {
        var methods = Set<String>()
        var methodCount = UInt32()
        let methodDescriptionList = protocol_copyMethodDescriptionList(p, false, true, &methodCount)
        for i in 0..<Int(methodCount) {
            let description = methodDescriptionList![i]
            XCTAssertNotNil(description.name?.description)
            methods.insert(description.name!.description)
        }
        free(methodDescriptionList)
        return methods
    }

    class func classMethodDescriptions(_ cls: Swift.AnyClass) -> Set<String> {
        var methods = Set<String>()
        var methodCount = UInt32()
        let methodList = class_copyMethodList(cls, &methodCount)
        for i in 0..<Int(methodCount) {
            let method = methodList![i]
            let selector = method_getName(method)
            methods.insert(selector.description)
        }
        free(methodList)
        return methods
    }

}
