#include <mbgl/style/expression/collator.hpp>

#include <sstream>

#import <Foundation/Foundation.h>

namespace mbgl {
namespace style {
namespace expression {

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

        return [nsLhs compare:nsRhs options:options range:compareRange locale:locale];
    }

    std::string resolvedLocale() const {
        return [locale localeIdentifier].UTF8String;
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

} // namespace expression
} // namespace style
} // namespace mbgl
