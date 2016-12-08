#import "MGLGeoJSONSourceBase.h"
#import "MGLGeoJSONSourceBase_Private.h"

const MGLGeoJSONSourceOption MGLGeoJSONSourceOptionClustered = @"MGLGeoJSONSourceOptionClustered";
const MGLGeoJSONSourceOption MGLGeoJSONSourceOptionClusterRadius = @"MGLGeoJSONSourceOptionClusterRadius";
const MGLGeoJSONSourceOption MGLGeoJSONSourceOptionMaximumZoomLevelForClustering = @"MGLGeoJSONSourceOptionMaximumZoomLevelForClustering";
const MGLGeoJSONSourceOption MGLGeoJSONSourceOptionMaximumZoomLevel = @"MGLGeoJSONSourceOptionMaximumZoomLevel";
const MGLGeoJSONSourceOption MGLGeoJSONSourceOptionMinimumZoomLevel = @"MGLGeoJSONSourceOptionMinimumZoomLevel";
const MGLGeoJSONSourceOption MGLGeoJSONSourceOptionBuffer = @"MGLGeoJSONSourceOptionBuffer";
const MGLGeoJSONSourceOption MGLGeoJSONSourceOptionSimplificationTolerance = @"MGLGeoJSONSourceOptionSimplificationTolerance";

@interface MGLGeoJSONSourceBase ()

@property (nonatomic, readwrite) NS_DICTIONARY_OF(MGLGeoJSONSourceOption, id) *options;

@end

@implementation MGLGeoJSONSourceBase


- (instancetype)initWithIdentifier:(NSString *)identifier options:(NS_DICTIONARY_OF(MGLGeoJSONSourceOption, id) *)options
{
    if (self = [super initWithIdentifier:identifier]) {
        self.options = options;
    }
    return self;
}

- (mbgl::style::GeoJSONOptions)geoJSONOptions
{
    auto mbglOptions = mbgl::style::GeoJSONOptions();
    
    if (id value = self.options[MGLGeoJSONSourceOptionMaximumZoomLevel]) {
        [self validateValue:value];
        mbglOptions.maxzoom = [value integerValue];
    }
    
    if (id value = self.options[MGLGeoJSONSourceOptionMinimumZoomLevel]) {
        [self validateValue:value];
        mbglOptions.minzoom = [value integerValue];
    }
    
    if (id value = self.options[MGLGeoJSONSourceOptionBuffer]) {
        [self validateValue:value];
        mbglOptions.buffer = [value integerValue];
    }
    
    if (id value = self.options[MGLGeoJSONSourceOptionSimplificationTolerance]) {
        [self validateValue:value];
        mbglOptions.tolerance = [value doubleValue];
    }
    
    if (id value = self.options[MGLGeoJSONSourceOptionClusterRadius]) {
        [self validateValue:value];
        mbglOptions.clusterRadius = [value integerValue];
    }
    
    if (id value = self.options[MGLGeoJSONSourceOptionMaximumZoomLevelForClustering]) {
        [self validateValue:value];
        mbglOptions.clusterMaxZoom = [value integerValue];
    }
    
    if (id value = self.options[MGLGeoJSONSourceOptionClustered]) {
        [self validateValue:value];
        mbglOptions.cluster = [value boolValue];
    }
    
    return mbglOptions;
}

- (void)validateValue:(id)value
{
    if (! [value isKindOfClass:[NSNumber class]])
    {
        [NSException raise:@"Value not handled" format:@"%@ is not an NSNumber", value];
    }
}


@end
