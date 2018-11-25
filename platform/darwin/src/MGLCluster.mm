#import "MGLFoundation_Private.h"
#import "MGLCluster_Private.h"
#import "MGLLoggingConfiguration_Private.h"
#import "MGLFeature.h"
#import <objc/runtime.h>

NSString * const MGLClusterBoolKey              = @"cluster";
NSString * const MGLClusterIdentifierKey        = @"cluster_id";
NSString * const MGLClusterCountKey             = @"point_count";
NSString * const MGLClusterCountAbbreviationKey = @"point_count_abbreviated";

const NSUInteger MGLClusterIdentifierInvalid = NSUIntegerMax;

BOOL MGLFeatureHasClusterAttribute(id<MGLFeature> feature) {
    NSNumber *isCluster = MGL_OBJC_DYNAMIC_CAST([feature attributeForKey:MGLClusterBoolKey], NSNumber);
    return [isCluster boolValue];
}

#pragma mark - Custom subclassing

NSString *MGLClusterSubclassNameForFeature(id<MGLFeature> feature) {
    NSString *className = NSStringFromClass([feature class]);
    NSString *subclassName = [className stringByAppendingString:@"_Cluster"];
    return subclassName;
}

/**
 Static free function used as implementation for custom subclasses (that conform
 to both `MGLFeature` and `MGLCluster`).
 */
static NSUInteger MGLClusterIdentifierIMP(id self, SEL _cmd) {
    
    id<MGLFeature> feature = MGL_OBJC_DYNAMIC_CAST_AS_PROTOCOL(self, MGLFeature);
    
    NSNumber *clusterNumber = MGL_OBJC_DYNAMIC_CAST([feature attributeForKey:MGLClusterIdentifierKey], NSNumber);
    MGLAssert(clusterNumber, @"Clusters should have a cluster_id");
    
    if (!clusterNumber) {
        return MGLClusterIdentifierInvalid;
    }
    
    NSUInteger identifier = [clusterNumber unsignedIntegerValue];
    MGLAssert(identifier <= UINT32_MAX, @"Cluster identifiers are 32bit");
    
    return identifier;
}

/**
 Static free function used as implementation for custom subclasses (that conform
 to both `MGLFeature` and `MGLCluster`).
 */
static NSUInteger MGLClusterPointCountIMP(id self, SEL _cmd) {
    id<MGLFeature> feature = MGL_OBJC_DYNAMIC_CAST_AS_PROTOCOL(self, MGLFeature);

    NSNumber *count = MGL_OBJC_DYNAMIC_CAST([feature attributeForKey:MGLClusterCountKey], NSNumber);
    MGLAssert(count, @"Clusters should have a point_count");
    
    return [count unsignedIntegerValue];
}

/**
 Static free function used as implementation for custom subclasses (that conform
 to both `MGLFeature` and `MGLCluster`).
 */
static NSString *MGLClusterPointCountAbbreviationIMP(id self, SEL _cmd) {
    id<MGLFeature> feature = MGL_OBJC_DYNAMIC_CAST_AS_PROTOCOL(self, MGLFeature);

    NSString *abbreviation = MGL_OBJC_DYNAMIC_CAST([feature attributeForKey:MGLClusterCountAbbreviationKey], NSString);
    MGLAssert(abbreviation, @"Clusters should have a point_count_abbreviated");
    
    if (!abbreviation) {
        return @"0";
    }
    
    return abbreviation;
}

static IMP MGLFeatureClusterIMPFromSelector(SEL selector) {
    if (selector == @selector(clusterIdentifier)) {
        return (IMP)MGLClusterIdentifierIMP;
    }
    else if (selector == @selector(clusterPointCount)) {
        return (IMP)MGLClusterPointCountIMP;
    }
    else if (selector == @selector(clusterPointCountAbbreviation)) {
        return (IMP)MGLClusterPointCountAbbreviationIMP;
    }
    MGLCAssert(0, @"Unrecognized selector: %@", NSStringFromSelector(selector));
    return NULL;
}

/**
 Converts a feature to a custom subclass that supports both `MGLFeature` and
 `MGLCluster`, or returns `nil` if the feature doesn't have the requisite cluster
 attributes.
 */
id<MGLFeature, MGLCluster> MGLConvertFeatureToClusterSubclass(id<MGLFeature> feature) {
    
    Protocol *clusterProtocol = @protocol(MGLCluster);
    
    if ([feature conformsToProtocol:clusterProtocol]) {
        return (id<MGLFeature, MGLCluster>)feature;
    }
    
    if (!MGLFeatureHasClusterAttribute(feature)) {
        return nil;
    }
    
    Class currentClass = [feature class];
    
    NSString *subclassName = MGLClusterSubclassNameForFeature(feature);
    
    // Does this new subclass already exist?
    Class clusterSubclass = NSClassFromString(subclassName);
    
    if (!clusterSubclass) {
        clusterSubclass = objc_allocateClassPair(currentClass, subclassName.UTF8String, 0);
        
        // Add protocol to this new subclass
        class_addProtocol(clusterSubclass, clusterProtocol);
        
        // Install protocol methods
        // Run through using `protocol_copyMethodDescriptionList` and the above
        // `MGLFeatureClusterIMPFromSelector` method. We do this so that we avoid
        // having to hand-craft the type encodings passed to `class_addMethod`.
        unsigned int count = 0;
        struct objc_method_description *methodDescriptionList = protocol_copyMethodDescriptionList(clusterProtocol, YES, YES, &count);

        for (unsigned int index = 0; index < count; index++) {
            struct objc_method_description desc = methodDescriptionList[index];
            
            IMP imp = MGLFeatureClusterIMPFromSelector(desc.name);
            
            if (imp) {
                class_addMethod(clusterSubclass, desc.name, imp, desc.types);
            }
        }
        
        free(methodDescriptionList); methodDescriptionList = NULL;
        
        objc_registerClassPair(clusterSubclass);
    }
    
    // Now change the class of the feature
    object_setClass(feature, clusterSubclass);
    MGLCAssert([feature conformsToProtocol:clusterProtocol], @"Feature subclass %@ should conform to MGLCluster", subclassName);

    id<MGLFeature, MGLCluster> cluster = (id<MGLFeature, MGLCluster>)feature;
    
    MGLCAssert([cluster respondsToSelector:@selector(clusterIdentifier)], @"Feature subclass %@ - missing selector `clusterIdentifier`", subclassName);
    MGLCAssert([cluster respondsToSelector:@selector(clusterPointCount)], @"Feature subclass %@ - missing selector `clusterPointCount`", subclassName);
    MGLCAssert([cluster respondsToSelector:@selector(clusterPointCountAbbreviation)], @"Feature subclass %@ - missing selector `clusterPointCountAbbreviation`", subclassName);
    
    return cluster;
}
