#import <Mapbox/Mapbox.h>
#import <XCTest/XCTest.h>

#import "../../ios/src/MGLMapAccessibilityElement.h"

@interface MGLMapAccessibilityElementTests : XCTestCase
@end

@implementation MGLMapAccessibilityElementTests

- (void)testFeatureLabels {
    MGLPointFeature *feature = [[MGLPointFeature alloc] init];
    feature.attributes = @{
        @"name": @"Local",
        @"name_en": @"English",
        @"name_es": @"Spanish",
        @"name_fr": @"French",
        @"name_tlh": @"Klingon",
    };
    MGLFeatureAccessibilityElement *element = [[MGLFeatureAccessibilityElement alloc] initWithAccessibilityContainer:self feature:feature];
    XCTAssertEqualObjects(element.accessibilityLabel, @"English", @"Accessibility label should be localized.");
    
    feature.attributes = @{
        @"name": @"Цинциннати",
        @"name_en": @"Цинциннати",
    };
    element = [[MGLFeatureAccessibilityElement alloc] initWithAccessibilityContainer:self feature:feature];
    XCTAssertEqualObjects(element.accessibilityLabel, @"Cincinnati", @"Accessibility label should be romanized.");
}

@end
