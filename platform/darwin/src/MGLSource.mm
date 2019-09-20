#import "MGLSource_Private.h"
#import "MGLStyle_Private.h"
#import "MGLMapView_Private.h"
#import "NSBundle+MGLAdditions.h"

#if TARGET_OS_IPHONE || TARGET_OS_SIMULATOR
#import "MMEEventsManager.h"
#endif

#include <mbgl/style/style.hpp>
#include <mbgl/map/map.hpp>
#include <mbgl/style/source.hpp>

const MGLExceptionName MGLInvalidStyleSourceException = @"MGLInvalidStyleSourceException";

@interface MGLSource ()

// Even though this class is abstract, MGLStyle uses it to represent some
// special internal source types like mbgl::AnnotationSource.
@property (nonatomic, readonly) mbgl::style::Source *rawSource;

@property (nonatomic, readonly, weak) MGLMapView *mapView;

@end

@implementation MGLSource {
    std::unique_ptr<mbgl::style::Source> _pendingSource;
    mapbox::base::WeakPtr<mbgl::style::Source> _weakSource;
}


- (instancetype)initWithIdentifier:(NSString *)identifier
{
    if (self = [super init]) {
        _identifier = [identifier copy];
    }
    return self;
}

- (instancetype)initWithRawSource:(mbgl::style::Source *)rawSource mapView:(MGLMapView *)mapView {
    NSString *identifier = @(rawSource->getID().c_str());
    if (self = [self initWithIdentifier:identifier]) {
        _weakSource = rawSource->makeWeakPtr();
        rawSource->peer = SourceWrapper { self };
        _mapView = mapView;
    }
    return self;
}

- (mbgl::style::Source *)rawSource
{
    return _weakSource.get();
}

- (instancetype)initWithPendingSource:(std::unique_ptr<mbgl::style::Source>)pendingSource {
    if (self = [self initWithRawSource:pendingSource.get() mapView:nil]) {
        _pendingSource = std::move(pendingSource);
    }
    return self;
}

- (void)addToMapView:(MGLMapView *)mapView {
    if (_pendingSource == nullptr) {
        [NSException raise:MGLRedundantSourceException
                    format:@"This instance %@ was already added to %@. Adding the same source instance " \
         "to the style more than once is invalid.", self, mapView.style];
    }
    
    _mapView = mapView;
    _mapView.style.rawStyle->addSource(std::move(_pendingSource));
}

- (BOOL)removeFromMapView:(MGLMapView *)mapView error:(NSError * __nullable * __nullable)outError {
    MGLAssertStyleSourceIsValid();
    BOOL removed = NO;
    
    if (self.rawSource == mapView.style.rawStyle->getSource(self.identifier.UTF8String)) {
        
        auto removedSource = mapView.style.rawStyle->removeSource(self.identifier.UTF8String);
        
        if (removedSource) {
            removed = YES;
            _pendingSource = std::move(removedSource);
            _mapView = nil;
        } else if (outError) {
            NSString *localizedDescription = [NSString stringWithFormat:
                                              NSLocalizedStringWithDefaultValue(@"REMOVE_SRC_FAIL_IN_USE_FMT", @"Foundation", nil, @"The source “%@” can’t be removed while it is in use.", @"User-friendly error description; first placeholder is the source’s identifier"),
                                              self.identifier];

            *outError = [NSError errorWithDomain:MGLErrorDomain
                                            code:MGLErrorCodeSourceIsInUseCannotRemove
                                        userInfo:@{ NSLocalizedDescriptionKey : localizedDescription }];
#if TARGET_OS_IPHONE || TARGET_OS_SIMULATOR
            [[MMEEventsManager sharedManager] reportError:*outError];
#endif
            
        }
    } else if (outError) {
        // TODO: Consider raising an exception here
        NSString *localizedDescription = [NSString stringWithFormat:
                                          NSLocalizedStringWithDefaultValue(@"REMOVE_SRC_FAIL_MISMATCH_FMT", @"Foundation", nil, @"The source can’t be removed because its identifier, “%@”, belongs to a different source in this style.", @"User-friendly error description"),
                                          self.identifier];
        
        *outError = [NSError errorWithDomain:MGLErrorDomain
                                        code:MGLErrorCodeSourceIdentifierMismatch
                                    userInfo:@{ NSLocalizedDescriptionKey : localizedDescription }];
#if TARGET_OS_IPHONE || TARGET_OS_SIMULATOR
        [[MMEEventsManager sharedManager] reportError:*outError];
#endif
    }
    
    return removed;
}

- (NSString *)description {
    return [NSString stringWithFormat:@"<%@: %p; identifier = %@>",
            NSStringFromClass([self class]), (void *)self, self.identifier];
}

@end
