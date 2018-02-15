#import "StyleLayerIconTransformer.h"

#import <Mapbox/Mapbox.h>

@implementation StyleLayerIconTransformer

+ (Class)transformedValueClass {
    return [NSString class];
}

+ (BOOL)allowsReverseTransformation {
    return NO;
}

- (id)transformedValue:(MGLStyleLayer *)layer {
    if ([layer isKindOfClass:[MGLBackgroundStyleLayer class]]) {
        return [NSImage imageNamed:@"background"];
    }
    if ([layer isKindOfClass:[MGLCircleStyleLayer class]]) {
        return [NSImage imageNamed:@"circle"];
    }
    if ([layer isKindOfClass:[MGLFillStyleLayer class]]) {
        return [NSImage imageNamed:@"fill"];
    }
    if ([layer isKindOfClass:[MGLLineStyleLayer class]]) {
        return [NSImage imageNamed:@"NSListViewTemplate"];
    }
    if ([layer isKindOfClass:[MGLRasterStyleLayer class]]) {
        return [[NSWorkspace sharedWorkspace] iconForFileType:@"jpg"];
    }
    if ([layer isKindOfClass:[MGLSymbolStyleLayer class]]) {
        return [NSImage imageNamed:@"symbol"];
    }
    if ([layer isKindOfClass:[MGLHeatmapStyleLayer class]]) {
        return [NSImage imageNamed:@"heatmap"];
    }
    if ([layer isKindOfClass:[MGLHillshadeStyleLayer class]]) {
        return [NSImage imageNamed:@"hillshade"];
    }
    
    return nil;
}

@end
