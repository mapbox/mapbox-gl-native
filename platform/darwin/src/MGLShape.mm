#import "MGLShape_Private.h"

#import "MGLFeature_Private.h"

@implementation MGLShape

+ (nullable MGLShape *)shapeWithData:(NSData *)data encoding:(NSStringEncoding)encoding error:(NSError * _Nullable *)outError {
    NSString *string = [[NSString alloc] initWithData:data encoding:encoding];
    if (!string) {
        if (outError) {
            *outError = [NSError errorWithDomain:MGLErrorDomain code:MGLErrorCodeUnknown userInfo:nil];
        }
        return nil;
    }
    
    try {
        const auto geojson = mapbox::geojson::parse(string.UTF8String);
        return MGLShapeFromGeoJSON(geojson);
    } catch (std::runtime_error &err) {
        if (outError) {
            *outError = [NSError errorWithDomain:MGLErrorDomain code:MGLErrorCodeUnknown userInfo:@{
                NSLocalizedFailureReasonErrorKey: @(err.what()),
            }];
        }
        return nil;
    }
}

- (mbgl::GeoJSON)geoJSONObject {
    return self.geometryObject;
}

- (mbgl::Geometry<double>)geometryObject {
    [NSException raise:@"MGLAbstractClassException"
                format:@"MGLShape is an abstract class"];
    return mbgl::Point<double>();
}

- (NSData *)geoJSONDataUsingEncoding:(NSStringEncoding)encoding {
    auto geometry = self.geoJSONObject;
    NSString *string = @(mapbox::geojson::stringify(geometry).c_str());
    return [string dataUsingEncoding:NSUTF8StringEncoding];
}

- (CLLocationCoordinate2D)coordinate {
    [NSException raise:@"MGLAbstractClassException"
                format:@"MGLShape is an abstract class"];

    return kCLLocationCoordinate2DInvalid;
}

@end
