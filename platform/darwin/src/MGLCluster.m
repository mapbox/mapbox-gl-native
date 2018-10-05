#import "MGLCluster_Private.h"
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

NSString *MGLClusterSubclassNameForFeature(id<MGLFeature> feature) {
    NSString *className = NSStringFromClass([feature class]);
    NSString *subclassName = [className stringByAppendingString:@"_Cluster"];
    return subclassName;
}

static NSUInteger MGLClusterIdentifierIMP(id self, SEL _cmd) {
    
    id<MGLFeature> feature = MGL_OBJC_DYNAMIC_CAST_AS_PROTOCOL(self, MGLFeature);
    
    NSNumber *clusterNumber = MGL_OBJC_DYNAMIC_CAST([feature attributeForKey:MGLClusterIdentifierKey], NSNumber);
    NSAssert(clusterNumber, @"Clusters should have a cluster_id");
    
    if (!clusterNumber) {
        return MGLClusterIdentifierInvalid;
    }
    
    NSUInteger identifier = [clusterNumber unsignedIntegerValue];
    NSAssert(identifier <= UINT32_MAX, @"Cluster identifiers are 32bit");
    
    return identifier;
}

static NSUInteger MGLClusterPointCountIMP(id self, SEL _cmd) {
    id<MGLFeature> feature = MGL_OBJC_DYNAMIC_CAST_AS_PROTOCOL(self, MGLFeature);

    NSNumber *count = MGL_OBJC_DYNAMIC_CAST([feature attributeForKey:MGLClusterCountKey], NSNumber);
    NSAssert(count, @"Clusters should have a point_count");
    
    return [count unsignedIntegerValue];
}

static NSString *MGLClusterPointCountAbbreviationIMP(id self, SEL _cmd) {
    id<MGLFeature> feature = MGL_OBJC_DYNAMIC_CAST_AS_PROTOCOL(self, MGLFeature);

    NSString *abbreviation = MGL_OBJC_DYNAMIC_CAST([feature attributeForKey:MGLClusterCountAbbreviationKey], NSString);
    NSAssert(abbreviation, @"Clusters should have a point_count_abbreviated");
    
    if (!abbreviation) {
        return @"0";
    }
    
    return abbreviation;
}

static IMP MGLFeatureClusterIMPFromSelector(SEL selector) {
    if (selector == NSSelectorFromString(@"clusterIdentifier")) {
        return (IMP)MGLClusterIdentifierIMP;
    }
    else if (selector == NSSelectorFromString(@"clusterPointCount")) {
        return (IMP)MGLClusterPointCountIMP;
    }
    else if (selector == NSSelectorFromString(@"clusterPointCountAbbreviation")) {
        return (IMP)MGLClusterPointCountAbbreviationIMP;
    }

    assert(0);
    return NULL;
}

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
    assert([feature conformsToProtocol:clusterProtocol]);

    id<MGLFeature, MGLCluster> cluster = (id<MGLFeature, MGLCluster>)feature;
    
    assert([cluster respondsToSelector:@selector(clusterIdentifier)]);
    assert([cluster respondsToSelector:@selector(clusterPointCount)]);
    assert([cluster respondsToSelector:@selector(clusterPointCountAbbreviation)]);
    
    return cluster;
}
