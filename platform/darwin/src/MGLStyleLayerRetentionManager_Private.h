@class MGLStyle;

static const NSInteger MGLStyleLayerRetentionManagerDefaultLifetime = 2;

/**
 Object use to manage the retain/release of `MGLStyleLayer`s (currently only `MGLOpenGLStyleLayer`.
 Managed layers are given a "lifetime", and this is reset everytime `-updateRetainedLayers:` is called.
 The lifetime is decremented with each call to `-decrementLifetimes`, when it reaches 0 the layer
 is removed from the manager (potentially releasing it)
 */
@interface MGLStyleLayerRetentionManager : NSObject
- (BOOL)isManagedLayer:(nullable MGLStyleLayer*)styleLayer;
- (void)updateRetainedLayers:(nonnull NSSet<MGLStyleLayer*>*)sourceObjects;
- (void)decrementLifetimes;
@end

