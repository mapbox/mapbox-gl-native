#include <mbgl/i18n/collator.hpp>

#include <sstream>

#import <Foundation/Foundation.h>

namespace mbgl {
namespace platform {

class Collator::Impl {
public:
    Impl(bool caseSensitive, bool diacriticSensitive, optional<std::string> locale_)
        : options((caseSensitive ? 0 : NSCaseInsensitiveSearch) |
                  (diacriticSensitive ? 0 : NSDiacriticInsensitiveSearch))
        , locale(locale_ ?
                    [[NSLocale alloc] initWithLocaleIdentifier:@((*locale_).c_str())] :
                    [NSLocale currentLocale])
    {}

    bool operator==(const Impl& other) const {
        return options == other.options &&
            [[locale localeIdentifier] isEqualToString:[other.locale localeIdentifier]];
    }

    int compare(const std::string& lhs, const std::string& rhs) const {
        NSString* nsLhs = @(lhs.c_str());
        NSString* nsRhs = @(rhs.c_str());
        // Limiting the compare range to the length of the LHS seems weird, but
        // experimentally we've checked that if LHS is a prefix of RHS compare returns -1
        // https://developer.apple.com/documentation/foundation/nsstring/1414561-compare
        NSRange compareRange = NSMakeRange(0, nsLhs.length);

        return static_cast<int>([nsLhs compare:nsRhs options:options range:compareRange locale:locale]);
    }

    std::string resolvedLocale() const {
        // Following documentation at:
        // https://developer.apple.com/library/archive/documentation/MacOSX/Conceptual/BPInternational/LanguageandLocaleIDs/LanguageandLocaleIDs.html#//apple_ref/doc/uid/10000171i-CH15-SW9
        // We expect NSLocale to accept BCP 47 tags as localeIdentifier inputs, but the output format
        // may append the region tag with an "_". Changing that to a "-" makes the identifier BCP 47 compliant.
        // Experimentally, "zh-Hans-HK" and "zh-Hans_HK" both round trip -- if the second is used by
        //  `currentLocale`, we don't want to return the underscore.
        return [[locale localeIdentifier] stringByReplacingOccurrencesOfString:@"_" withString:@"-"].UTF8String;
    }
private:
    NSStringCompareOptions options;
    NSLocale* locale;
};

Collator::Collator(bool caseSensitive, bool diacriticSensitive, optional<std::string> locale_)
    : impl(std::make_shared<Impl>(caseSensitive, diacriticSensitive, std::move(locale_)))
{}

bool Collator::operator==(const Collator& other) const {
    return *impl == *(other.impl);
}

int Collator::compare(const std::string& lhs, const std::string& rhs) const {
    return impl->compare(lhs, rhs);
}

std::string Collator::resolvedLocale() const {
    return impl->resolvedLocale();
}

} // namespace platform
} // namespace mbgl
