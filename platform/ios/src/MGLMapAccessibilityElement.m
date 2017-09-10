#import "MGLMapAccessibilityElement.h"
#import "MGLDistanceFormatter.h"
#import "MGLCompassDirectionFormatter.h"
#import "MGLFeature.h"
#import "MGLVectorSource+MGLAdditions.h"

#import "NSBundle+MGLAdditions.h"

typedef CLLocationDegrees MGLLocationRadians;
typedef CLLocationDirection MGLRadianDirection;
typedef struct {
    MGLLocationRadians latitude;
    MGLLocationRadians longitude;
} MGLRadianCoordinate2D;

/** Returns the direction from one coordinate to another. */
CLLocationDirection MGLDirectionBetweenCoordinates(CLLocationCoordinate2D firstCoordinate, CLLocationCoordinate2D secondCoordinate) {
    MGLRadianCoordinate2D firstRadianCoordinate = {
        firstCoordinate.latitude * M_PI / 180,
        firstCoordinate.longitude * M_PI / 180,
    };
    MGLRadianCoordinate2D secondRadianCoordinate = {
        secondCoordinate.latitude * M_PI / 180,
        secondCoordinate.longitude * M_PI / 180,
    };
    
    CGFloat a = sin(secondRadianCoordinate.longitude - firstRadianCoordinate.longitude) * cos(secondRadianCoordinate.latitude);
    CGFloat b = (cos(firstRadianCoordinate.latitude) * sin(secondRadianCoordinate.latitude)
                 - sin(firstRadianCoordinate.latitude) * cos(secondRadianCoordinate.latitude) * cos(secondRadianCoordinate.longitude - firstRadianCoordinate.longitude));
    MGLRadianDirection radianDirection = atan2(a, b);
    return radianDirection * 180 / M_PI;
}

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
        
        NSString *nameAttribute = [NSString stringWithFormat:@"name_%@",
                                   [MGLVectorSource preferredMapboxStreetsLanguage]];
        self.accessibilityLabel = [feature attributeForKey:nameAttribute];
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
        
        NSNumber *elevation = attributes[@"elevation_m"];
        if (elevation) {
            MGLDistanceFormatter *formatter = [[MGLDistanceFormatter alloc] init];
            formatter.unitStyle = NSFormattingUnitStyleLong;
            [facts addObject:[formatter stringFromDistance:elevation.doubleValue]];
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
        
        // Announce whether the road is a divided road.
        if ([feature isKindOfClass:[MGLShapeCollectionFeature class]]) {
            [facts addObject:NSLocalizedStringWithDefaultValue(@"ROAD_DIVIDED_A11Y_VALUE", nil, nil, @"Divided road", @"Accessibility value indicating that a road is a divided road (dual carriageway)")];
            feature = [(MGLShapeCollectionFeature *)feature shapes].firstObject;
        }
        
        // Announce the road’s general direction.
        if ([feature isKindOfClass:[MGLPolylineFeature class]]) {
            NSUInteger pointCount = [(MGLPolylineFeature *)feature pointCount];
            if (pointCount) {
                CLLocationCoordinate2D *coordinates = [(MGLPolyline *)feature coordinates];
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
