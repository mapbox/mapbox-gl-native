#import "MGLMapAccessibilityElement.h"
#import "MGLDistanceFormatter.h"
#import "MGLCompassDirectionFormatter.h"
#import "MGLFeature.h"

#import "MGLGeometry_Private.h"
#import "MGLVectorSource_Private.h"

#import "NSBundle+MGLAdditions.h"
#import "NSOrthography+MGLAdditions.h"
#import "NSString+MGLAdditions.h"

@implementation MGLMapAccessibilityElement

- (UIAccessibilityTraits)accessibilityTraits {
    return super.accessibilityTraits | UIAccessibilityTraitAdjustable;
}

- (void)accessibilityIncrement {
    [self.accessibilityContainer accessibilityIncrement];
}

- (void)accessibilityDecrement {
    [self.accessibilityContainer accessibilityDecrement];
}

@end

@implementation MGLAnnotationAccessibilityElement

- (instancetype)initWithAccessibilityContainer:(id)container tag:(MGLAnnotationTag)tag {
    if (self = [super initWithAccessibilityContainer:container]) {
        _tag = tag;
        self.accessibilityHint = NSLocalizedStringWithDefaultValue(@"ANNOTATION_A11Y_HINT", nil, nil, @"Shows more info", @"Accessibility hint");
    }
    return self;
}

- (UIAccessibilityTraits)accessibilityTraits {
    return super.accessibilityTraits | UIAccessibilityTraitButton;
}

@end

@implementation MGLFeatureAccessibilityElement

- (instancetype)initWithAccessibilityContainer:(id)container feature:(id<MGLFeature>)feature {
    if (self = [super initWithAccessibilityContainer:container]) {
        _feature = feature;
        
        NSString *languageCode = [MGLVectorSource preferredMapboxStreetsLanguage];
        NSString *nameAttribute = [NSString stringWithFormat:@"name_%@", languageCode];
        NSString *name = [feature attributeForKey:nameAttribute];

        // If a feature hasn’t been translated into the preferred language, it
        // may be in the local language, which may be written in another script.
        // Attempt to transform to the script of the preferred language, keeping
        // the original string if no transform exists or if transformation fails.
        if (@available(iOS 9.0, *)) {
            NSString *dominantScript = [NSOrthography mgl_dominantScriptForMapboxStreetsLanguage:languageCode];
            name = [name mgl_stringByTransliteratingIntoScript:dominantScript];
        }

        self.accessibilityLabel = name;
    }
    return self;
}

- (UIAccessibilityTraits)accessibilityTraits {
    return super.accessibilityTraits | UIAccessibilityTraitStaticText;
}

@end

@implementation MGLPlaceFeatureAccessibilityElement

- (instancetype)initWithAccessibilityContainer:(id)container feature:(id<MGLFeature>)feature {
    if (self = [super initWithAccessibilityContainer:container feature:feature]) {
        NSDictionary *attributes = feature.attributes;
        NSMutableArray *facts = [NSMutableArray array];
        
        // Announce the kind of place or POI.
        if (attributes[@"type"]) {
            // FIXME: Unfortunately, these types aren’t a closed set that can be
            // localized, since they’re based on OpenStreetMap tags.
            NSString *type = [attributes[@"type"] stringByReplacingOccurrencesOfString:@"_"
                                                                            withString:@" "];
            [facts addObject:type];
        }
        // Announce the kind of airport, rail station, or mountain based on its
        // Maki image name.
        else if (attributes[@"maki"]) {
            // TODO: Localize Maki image names.
            [facts addObject:attributes[@"maki"]];
        }
        
        // Announce the peak’s elevation in the preferred units.
        if (attributes[@"elevation_m"] ?: attributes[@"elevation_ft"]) {
            NSLengthFormatter *formatter = [[NSLengthFormatter alloc] init];
            formatter.unitStyle = NSFormattingUnitStyleLong;
            
            NSNumber *elevationValue;
            NSLengthFormatterUnit unit;
            BOOL usesMetricSystem = ![[formatter.numberFormatter.locale objectForKey:NSLocaleMeasurementSystem]
                                      isEqualToString:@"U.S."];
            if (usesMetricSystem) {
                elevationValue = attributes[@"elevation_m"];
                unit = NSLengthFormatterUnitMeter;
            } else {
                elevationValue = attributes[@"elevation_ft"];
                unit = NSLengthFormatterUnitFoot;
            }
            [facts addObject:[formatter stringFromValue:elevationValue.doubleValue unit:unit]];
        }
        
        if (facts.count) {
            NSString *separator = NSLocalizedStringWithDefaultValue(@"LIST_SEPARATOR", nil, nil, @", ", @"List separator");
            self.accessibilityValue = [facts componentsJoinedByString:separator];
        }
    }
    return self;
}

@end

@implementation MGLRoadFeatureAccessibilityElement

- (instancetype)initWithAccessibilityContainer:(id)container feature:(id<MGLFeature>)feature {
    if (self = [super initWithAccessibilityContainer:container feature:feature]) {
        NSDictionary *attributes = feature.attributes;
        NSMutableArray *facts = [NSMutableArray array];
        
        // Announce the route number.
        if (attributes[@"ref"]) {
            // TODO: Decorate the route number with the network name based on the shield attribute.
            NSString *ref = [NSString stringWithFormat:NSLocalizedStringWithDefaultValue(@"ROAD_REF_A11Y_FMT", nil, nil, @"Route %@", @"String format for accessibility value for road feature; {route number}"), attributes[@"ref"]];
            [facts addObject:ref];
        }
        
        // Announce whether the road is a one-way road.
        if ([attributes[@"oneway"] isEqualToString:@"true"]) {
            [facts addObject:NSLocalizedStringWithDefaultValue(@"ROAD_ONEWAY_A11Y_VALUE", nil, nil, @"One way", @"Accessibility value indicating that a road is a one-way road")];
        }
        
        // Announce whether the road is a divided road.
        MGLPolyline *polyline;
        if ([feature isKindOfClass:[MGLMultiPolylineFeature class]]) {
            [facts addObject:NSLocalizedStringWithDefaultValue(@"ROAD_DIVIDED_A11Y_VALUE", nil, nil, @"Divided road", @"Accessibility value indicating that a road is a divided road (dual carriageway)")];
            polyline = [(MGLMultiPolylineFeature *)feature polylines].firstObject;
        }
        
        // Announce the road’s general direction.
        if ([feature isKindOfClass:[MGLPolylineFeature class]]) {
            polyline = (MGLPolylineFeature *)feature;
        }
        if (polyline) {
            NSUInteger pointCount = polyline.pointCount;
            if (pointCount) {
                CLLocationCoordinate2D *coordinates = polyline.coordinates;
                CLLocationDirection startDirection = MGLDirectionBetweenCoordinates(coordinates[pointCount - 1], coordinates[0]);
                CLLocationDirection endDirection = MGLDirectionBetweenCoordinates(coordinates[0], coordinates[pointCount - 1]);
                
                MGLCompassDirectionFormatter *formatter = [[MGLCompassDirectionFormatter alloc] init];
                formatter.unitStyle = NSFormattingUnitStyleLong;
                
                NSString *startDirectionString = [formatter stringFromDirection:startDirection];
                NSString *endDirectionString = [formatter stringFromDirection:endDirection];
                NSString *directionString = [NSString stringWithFormat:NSLocalizedStringWithDefaultValue(@"ROAD_DIRECTION_A11Y_FMT", nil, nil, @"%@ to %@", @"String format for accessibility value for road feature; {starting compass direction}, {ending compass direction}"), startDirectionString, endDirectionString];
                [facts addObject:directionString];
            }
        }
        
        if (facts.count) {
            NSString *separator = NSLocalizedStringWithDefaultValue(@"LIST_SEPARATOR", nil, nil, @", ", @"List separator");
            self.accessibilityValue = [facts componentsJoinedByString:separator];
        }
    }
    return self;
}

@end

@implementation MGLMapViewProxyAccessibilityElement

- (instancetype)initWithAccessibilityContainer:(id)container {
    if (self = [super initWithAccessibilityContainer:container]) {
        self.accessibilityTraits = UIAccessibilityTraitButton;
        self.accessibilityLabel = [self.accessibilityContainer accessibilityLabel];
        self.accessibilityHint = NSLocalizedStringWithDefaultValue(@"CLOSE_CALLOUT_A11Y_HINT", nil, nil, @"Returns to the map", @"Accessibility hint for closing the selected annotation’s callout view and returning to the map");
    }
    return self;
}

@end
