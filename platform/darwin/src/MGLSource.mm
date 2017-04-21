#import "MGLSource_Private.h"
#import "MGLStyle_Private.h"

#include <mbgl/style/style.hpp>
#include <mbgl/style/source.hpp>

@interface MGLSource ()

// Even though this class is abstract, MGLStyle uses it to represent some
// special internal source types like mbgl::AnnotationSource.
@property (nonatomic, readonly) mbgl::style::Source *rawSource;

@property (nonatomic, readonly, weak) MGLStyle *style;

@end

@implementation MGLSource {
    std::unique_ptr<mbgl::style::Source> _pendingSource;
}


- (instancetype)initWithIdentifier:(NSString *)identifier
{
    if (self = [super init]) {
        _identifier = identifier;
    }
    return self;
}

- (instancetype)initWithRawSource:(mbgl::style::Source *)rawSource {
    NSString *identifier = @(rawSource->getID().c_str());
    if (self = [self initWithIdentifier:identifier]) {
        _rawSource = rawSource;
        _rawSource->peer = SourceWrapper { self };
    }
    return self;
}

- (instancetype)initWithPendingSource:(std::unique_ptr<mbgl::style::Source>)pendingSource {
    if (self = [self initWithRawSource:pendingSource.get()]) {
        _pendingSource = std::move(pendingSource);
    }
    return self;
}

- (void)addToStyle:(MGLStyle *)style {
    if (_pendingSource == nullptr) {
        [NSException raise:@"MGLRedundantSourceException"
                    format:@"This instance %@ was already added to %@. Adding the same source instance " \
                            "to the style more than once is invalid.", self, style];
    }

    _style = style;
    style.rawStyle->addSource(std::move(_pendingSource));
}

- (void)removeFromStyle:(MGLStyle *)style {
    if (self.rawSource == style.rawStyle->getSource(self.identifier.UTF8String)) {
        _pendingSource = style.rawStyle->removeSource(self.identifier.UTF8String);
        _style = nil;
    }
}

- (NSString *)description {
    return [NSString stringWithFormat:@"<%@: %p; identifier = %@>",
            NSStringFromClass([self class]), (void *)self, self.identifier];
}

@end
