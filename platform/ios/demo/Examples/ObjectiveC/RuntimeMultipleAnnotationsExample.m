#import "RuntimeMultipleAnnotationsExample.h"
@import Mapbox;

NSString *const MBXExampleRuntimeMultipleAnnotations = @"RuntimeMultipleAnnotationsExample";

@interface RuntimeMultipleAnnotationsExample ()<MGLMapViewDelegate>
@property (nonatomic) MGLMapView *mapView;
@end

@implementation RuntimeMultipleAnnotationsExample

- (void)viewDidLoad {
    [super viewDidLoad];

    MGLMapView *mapView = [[MGLMapView alloc] initWithFrame:self.view.bounds];

    mapView.autoresizingMask = UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight;

    [mapView setCenterCoordinate:CLLocationCoordinate2DMake(37.090240, -95.712891) zoomLevel:2 animated:NO];

    mapView.delegate = self;

    // Add our own gesture recognizer to handle taps on our custom map features.
    [mapView addGestureRecognizer:[[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(handleMapTap:)]];

    [self.view addSubview:mapView];

    self.mapView = mapView;
}

- (void)mapView:(MGLMapView *)mapView didFinishLoadingStyle:(MGLStyle *)style {
    [self fetchPoints:^(NSArray *features) {
        [self addItemsToMap:features];
    }];
}

- (void)addItemsToMap:(NSArray *)features {
    // You can add custom UIImages to the map style.
    // These can be referenced by an MGLSymbolStyleLayer’s iconImage property.
    [self.mapView.style setImage:[UIImage imageNamed:@"lighthouse"] forName:@"lighthouse"];

    // Add the features to the map as a MGLShapeSource.
    MGLShapeSource *source = [[MGLShapeSource alloc] initWithIdentifier:@"lighthouses" features:features options:nil];
    [self.mapView.style addSource:source];

    UIColor *lighthouseColor = [UIColor colorWithRed:0.08 green:0.44 blue:0.96 alpha:1.0];

    // Use MGLCircleStyleLayer to represent the points with simple circles.
    // In this case, we can use style functions to gradually change properties between zoom level 2 and 7: the circle opacity from 50% to 100% and the circle radius from 2pt to 3pt.
    MGLCircleStyleLayer *circles = [[MGLCircleStyleLayer alloc] initWithIdentifier:@"lighthouse-circles" source:source];
    circles.circleColor = [MGLStyleValue valueWithRawValue:lighthouseColor];
    circles.circleOpacity = [MGLStyleValue valueWithInterpolationMode:MGLInterpolationModeExponential
        cameraStops:@{
            @2: [MGLStyleValue valueWithRawValue:@0.5],
            @7: [MGLStyleValue valueWithRawValue:@1.0]
        }
        options:@{MGLStyleFunctionOptionDefaultValue:[MGLStyleValue valueWithRawValue:@0.75]}];
                             
    circles.circleRadius = [MGLStyleValue valueWithInterpolationMode:MGLInterpolationModeInterval
        cameraStops:@{
            @2: [MGLStyleValue valueWithRawValue:@2],
            @7: [MGLStyleValue valueWithRawValue:@3]
        }
        options:@{MGLStyleFunctionOptionDefaultValue:@1}];
    
    // Use MGLSymbolStyleLayer for more complex styling of points including custom icons and text rendering.
    MGLSymbolStyleLayer *symbols = [[MGLSymbolStyleLayer alloc] initWithIdentifier:@"lighthouse-symbols" source:source];
    symbols.iconImageName = [MGLStyleValue valueWithRawValue:@"lighthouse"];
    symbols.iconScale = [MGLStyleValue valueWithRawValue:@0.5];
    symbols.iconOpacity = [MGLStyleValue valueWithInterpolationMode:MGLInterpolationModeExponential
        cameraStops:@{
            @5.9: [MGLStyleValue valueWithRawValue:@0],
            @6: [MGLStyleValue valueWithRawValue:@1],
        }
        options:nil];
    symbols.iconHaloColor = [MGLStyleValue valueWithRawValue:[[UIColor whiteColor] colorWithAlphaComponent:0.5]];
    symbols.iconHaloWidth = [MGLStyleValue valueWithRawValue:@1];
    // {name} references the "name" key in an MGLPointFeature’s attributes dictionary.
    symbols.text = [MGLStyleValue valueWithRawValue:@"{name}"];
    symbols.textColor = symbols.iconColor;
    symbols.textFontSize = [MGLStyleValue valueWithInterpolationMode:MGLInterpolationModeExponential
        cameraStops:@{
            @10: [MGLStyleValue valueWithRawValue:@10],
            @16: [MGLStyleValue valueWithRawValue:@16],
        }
        options:nil];
    symbols.textTranslation = [MGLStyleValue valueWithRawValue:[NSValue valueWithCGVector:CGVectorMake(10, 0)]];
    symbols.textOpacity = symbols.iconOpacity;
    symbols.textHaloColor = symbols.iconHaloColor;
    symbols.textHaloWidth = symbols.iconHaloWidth;
    symbols.textJustification = [MGLStyleValue valueWithRawValue:[NSValue valueWithMGLTextJustification:MGLTextJustificationLeft]];
    symbols.textAnchor = [MGLStyleValue valueWithRawValue:[NSValue valueWithMGLTextAnchor:MGLTextAnchorLeft]];

    [self.mapView.style addLayer:circles];
    [self.mapView.style addLayer:symbols];
}

#pragma mark - Feature interaction

- (void)handleMapTap:(UITapGestureRecognizer *)sender {
    if (sender.state == UIGestureRecognizerStateEnded) {
        // Limit feature selection to just the following layer identifiers.
        NSArray *layerIdentifiers = @[@"lighthouse-symbols", @"lighthouse-circles"];

        CGPoint point = [sender locationInView:sender.view];

        // Try matching the exact point first
        for (id f in [self.mapView visibleFeaturesAtPoint:point inStyleLayersWithIdentifiers:[NSSet setWithArray:layerIdentifiers]]) {
            if ([f isKindOfClass:[MGLPointFeature class]]) {
                [self showCallout:f];
                return;
            }
        }

        // Otherwise, get first features within a rect the size of a touch (44x44).
        CGRect pointRect = {point, CGSizeZero};
        CGRect touchRect = CGRectInset(pointRect, -22.0, -22.0);
        for (id f in [self.mapView visibleFeaturesInRect:touchRect inStyleLayersWithIdentifiers:[NSSet setWithArray:layerIdentifiers]]) {
            if ([f isKindOfClass:[MGLPointFeature class]]) {
                [self showCallout:f];
                return;
            }
        }

        // If no features were found, deselect the selected annotation, if any.
        [self.mapView deselectAnnotation:[[self.mapView selectedAnnotations] firstObject] animated:YES];
    }
}

- (void)showCallout:(MGLPointFeature *)feature {
    MGLPointFeature *point = [[MGLPointFeature alloc] init];
    point.title = feature.attributes[@"name"];
    point.coordinate = feature.coordinate;

    // Selecting an feature that doesn’t already exist on the map will add a new annotation view.
    // We’ll need to use the map’s delegate methods to add an empty annotation view and remove it when we’re done selecting it.
    [self.mapView selectAnnotation:point animated:YES];
}

#pragma mark - MGLMapViewDelegate

- (BOOL)mapView:(MGLMapView *)mapView annotationCanShowCallout:(id <MGLAnnotation>)annotation {
    return YES;
}

- (void)mapView:(MGLMapView *)mapView didDeselectAnnotation:(id <MGLAnnotation>)annotation {
    [mapView removeAnnotation:annotation];
}

- (MGLAnnotationView *)mapView:(MGLMapView *)mapView viewForAnnotation:(id <MGLAnnotation>)annotation {
    // Create an empty view annotation. Set a frame to offset the callout.
    return [[MGLAnnotationView alloc] initWithFrame:CGRectMake(0, 0, 20, 20)];
}

#pragma mark - Data fetching and parsing

- (void)fetchPoints:(void (^)(NSArray *))completion {
    // Wikidata query for all lighthouses in the United States: http://tinyurl.com/zrl2jc4
    NSString *query = @"SELECT DISTINCT ?item "
	"?itemLabel ?coor ?image "
	"WHERE "
	    "{ "
	    "?item wdt:P31 wd:Q39715 . "
	    "?item wdt:P17 wd:Q30 . "
	    "?item wdt:P625 ?coor . "
	    "OPTIONAL { ?item wdt:P18 ?image } . "
	    "SERVICE wikibase:label { bd:serviceParam wikibase:language \"en\" } "
	"} "
	"ORDER BY ?itemLabel";

    NSMutableCharacterSet *characterSet = [[NSCharacterSet URLQueryAllowedCharacterSet] mutableCopy];
    [characterSet removeCharactersInString:@"?"];
    [characterSet removeCharactersInString:@"&"];
    [characterSet removeCharactersInString:@":"];

    NSString *encodedQuery = [query stringByAddingPercentEncodingWithAllowedCharacters:characterSet];

    NSString *urlString = [NSString stringWithFormat:@"https://query.wikidata.org/sparql?query=%@&format=json", encodedQuery];

    [[[NSURLSession sharedSession] dataTaskWithURL:[NSURL URLWithString:urlString] completionHandler:^(NSData *data, NSURLResponse *response, NSError *error) {
        if (!data) return;

        NSDictionary *json = [NSJSONSerialization JSONObjectWithData:data options:0 error:nil];
        NSArray *items = json[@"results"][@"bindings"];

        if (!items) return;

        dispatch_async(dispatch_get_main_queue(), ^{
            completion([self parseJSONItems:items]);
        });
    }] resume];
}

- (NSArray *)parseJSONItems:(NSArray *)items {
    NSMutableArray *features = [NSMutableArray array];
    for (NSDictionary *item in items) {
        NSString *title = item[@"itemLabel"][@"value"];
        NSString *point = item[@"coor"][@"value"];
        if (!item || !point) continue;

        NSString *parsedPoint = [[point stringByReplacingOccurrencesOfString:@"Point(" withString:@""] stringByReplacingOccurrencesOfString:@")" withString:@""];
        NSArray *pointComponents = [parsedPoint componentsSeparatedByString:@" "];

        MGLPointFeature *feature = [[MGLPointFeature alloc] init];
        feature.coordinate = CLLocationCoordinate2DMake([pointComponents[1] doubleValue], [pointComponents[0] doubleValue]);
        feature.title = title;
        // A feature’s attributes can used by runtime styling for things like text labels.
        feature.attributes = @{
            @"name": title,
        };
        [features addObject:feature];
    }
    return features;
}

@end
