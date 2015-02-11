#import "MGLStyleFunctionValue.h"

#import "MGLTypes.h"

@interface MGLStyleFunctionValue ()

@property (nonatomic) NSString *functionType;
@property (nonatomic) NSDictionary *stops;
@property (nonatomic) CGFloat zBase;
@property (nonatomic) CGFloat val;
@property (nonatomic) CGFloat slope;
@property (nonatomic) CGFloat min;
@property (nonatomic) CGFloat max;
@property (nonatomic) CGFloat minimumZoom;
@property (nonatomic) CGFloat maximumZoom;

- (NSDictionary *)rawStyle;

@end

@implementation MGLStyleFunctionValue

- (id)initWithFunctionType:(NSString *)functionType
                     stops:(NSDictionary *)stops
                     zBase:(CGFloat)zBase
                       val:(CGFloat)val
                     slope:(CGFloat)slope
                       min:(CGFloat)min
                       max:(CGFloat)max
               minimumZoom:(CGFloat)minimumZoom
               maximumZoom:(CGFloat)maximumZoom
{
    self = [super init];

    if (self)
    {
        _functionType = functionType;
        _stops = stops;
        _zBase = zBase;
        _val = val;
        _slope = slope;
        _min = min;
        _max = max;
        _minimumZoom = minimumZoom;
        _maximumZoom = maximumZoom;
    }

    return self;
}

- (id)rawStyle
{
    if ([self.functionType isEqualToString:MGLStyleValueTypeFunctionMinimumZoom])
    {
        return @[ @"min", @(self.minimumZoom) ];
    }
    else if ([self.functionType isEqualToString:MGLStyleValueTypeFunctionMaximumZoom])
    {
        return @[ @"max", @(self.maximumZoom) ];
    }
    else if ([self.functionType isEqualToString:MGLStyleValueTypeFunctionLinear])
    {
        return @[ @"linear", @(self.zBase), @(self.val), @(self.slope), @(self.min), @(self.max) ];
    }
    else if ([self.functionType isEqualToString:MGLStyleValueTypeFunctionExponential])
    {
        return @[ @"exponential", @(self.zBase), @(self.val), @(self.slope), @(self.min), @(self.max) ];
    }
    else if ([self.functionType isEqualToString:MGLStyleValueTypeFunctionStops])
    {
        NSMutableArray *returnArray = [NSMutableArray array];

        for (NSNumber *z in [self.stops allKeys])
        {
            [returnArray addObject:@{ @"z" : z, @"val" : self.stops[z] }];
        }

        [returnArray insertObject:@"stops" atIndex:0];

        return returnArray;
    }

    return nil;
}

+ (instancetype)stopsFunctionWithZoomLevelsAndValues:(NSNumber *)firstZoom, ...
{
    NSMutableArray *numbersArray = [NSMutableArray array];

    va_list args;
    va_start(args, firstZoom);

    for (NSNumber *arg = firstZoom; arg != nil; arg = va_arg(args, NSNumber *))
    {
        [numbersArray addObject:arg];
    }

    va_end(args);

    NSAssert([numbersArray count] % 2 == 0, @"invalid number of arguments");

    NSMutableDictionary *stops = [NSMutableDictionary dictionary];

    for (NSUInteger i = 0; i < [numbersArray count]; i = i + 2)
    {
        stops[numbersArray[i]] = stops[numbersArray[i + 1]];
    }

    return [[self alloc] initWithFunctionType:MGLStyleValueTypeFunctionStops
                                        stops:stops
                                        zBase:0
                                          val:0
                                        slope:0
                                          min:0
                                          max:0
                                  minimumZoom:0
                                  maximumZoom:0];
}

+ (instancetype)linearFunctionWithBaseZoomLevel:(CGFloat)zBase
                                   initialValue:(CGFloat)val
                                          slope:(CGFloat)slope
                                   minimumValue:(CGFloat)min
                                   maximumValue:(CGFloat)max
{
    NSAssert(zBase >= 0 && zBase <= 18, @"invalid base zoom level");
    NSAssert(min < max, @"minimum value must be less than maximum value");

    return [[self alloc] initWithFunctionType:MGLStyleValueTypeFunctionLinear
                                        stops:nil
                                        zBase:zBase
                                          val:val
                                        slope:slope
                                          min:min
                                          max:max
                                  minimumZoom:0
                                  maximumZoom:0];
}

+ (instancetype)exponentialFunctionWithBaseZoomLevel:(CGFloat)zBase
                                        initialValue:(CGFloat)val
                                               slope:(CGFloat)slope
                                        minimumValue:(CGFloat)min
                                        maximumValue:(CGFloat)max
{
    NSAssert(zBase >= 0 && zBase <= 18, @"invalid base zoom level");
    NSAssert(min < max, @"minimum value must be less than maximum value");

    return [[self alloc] initWithFunctionType:MGLStyleValueTypeFunctionExponential
                                        stops:nil
                                        zBase:zBase
                                          val:val
                                        slope:slope
                                          min:min
                                          max:max
                                  minimumZoom:0
                                  maximumZoom:0];
}

+ (instancetype)minimumZoomLevelFunction:(CGFloat)minimumZoom
{
    NSAssert(minimumZoom >= 0 && minimumZoom <= 18, @"invalid minimum zoom value");

    return [[self alloc] initWithFunctionType:MGLStyleValueTypeFunctionMinimumZoom
                                        stops:nil
                                        zBase:0
                                          val:0
                                        slope:0
                                          min:0
                                          max:0
                                  minimumZoom:minimumZoom
                                  maximumZoom:0];
}

+ (instancetype)maximumZoomLevelFunction:(CGFloat)maximumZoom
{
    NSAssert(maximumZoom >= 0 && maximumZoom <= 18, @"invalid maximum zoom value");

    return [[self alloc] initWithFunctionType:MGLStyleValueTypeFunctionMaximumZoom
                                        stops:nil
                                        zBase:0
                                          val:0
                                        slope:0
                                          min:0
                                          max:0
                                  minimumZoom:0
                                  maximumZoom:maximumZoom];
}

@end
