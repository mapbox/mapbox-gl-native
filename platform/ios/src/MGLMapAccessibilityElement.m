#import "MGLMapAccessibilityElement.h"
#import "MGLDistanceFormatter.h"
#import "MGLFeature.h"
#import "MGLVectorSource+MGLAdditions.h"

#import "NSBundle+MGLAdditions.h"

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
