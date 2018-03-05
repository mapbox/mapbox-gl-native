#import "MGLStyle.h"
#import "MGLStyleLayer.h"
#import "MGLStyleLayerRetentionManager_Private.h"
#include <mbgl/style/style.hpp>
#include <mbgl/style/layers/custom_layer.hpp>

static const NSUInteger MGLStyleLayerRetentionManagerCapacityHint = 100;

@interface MGLStyleLayerRetentionManager ()
@property (nonatomic) NSMapTable<MGLStyleLayer*, NSNumber*> *retentionTable;
@end

@implementation MGLStyleLayerRetentionManager

- (instancetype)init {
    if ((self = [super init])) {
        _retentionTable = [[NSMapTable alloc] initWithKeyOptions:NSPointerFunctionsStrongMemory|NSPointerFunctionsObjectPointerPersonality
                                                    valueOptions:NSPointerFunctionsStrongMemory
                                                        capacity:MGLStyleLayerRetentionManagerCapacityHint];
    }
    return self;
}

- (BOOL)isManagedLayer:(nullable MGLStyleLayer*)styleLayer {
    return [self.retentionTable objectForKey:styleLayer] != nil;
}

- (void)updateRetainedLayers:(nonnull NSSet<MGLStyleLayer*>*)sourceObjects {

    // Add/update the objects from the source set, with a default lifetime
    for (id layer in sourceObjects) {
        [self.retentionTable setObject:@(MGLStyleLayerRetentionManagerDefaultLifetime) forKey:layer];
    }
}

- (void)decrementLifetimes {
    // Consider double-buffering the two tables, so we don't keep allocing/deallocing tables.
    NSMapTable *retentionTable = [[NSMapTable alloc] initWithKeyOptions:NSPointerFunctionsStrongMemory|NSPointerFunctionsObjectPointerPersonality
                                                           valueOptions:NSPointerFunctionsStrongMemory
                                                               capacity:MGLStyleLayerRetentionManagerCapacityHint];

    for (MGLStyleLayer *layer in self.retentionTable) {
        NSInteger lifeTime = [[self.retentionTable objectForKey:layer] integerValue];

        if (lifeTime > 0) {
            [retentionTable setObject:@(lifeTime - 1) forKey:layer];
        }
    }

    self.retentionTable = retentionTable;
}

@end
