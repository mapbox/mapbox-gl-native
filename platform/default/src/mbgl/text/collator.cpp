#include <mbgl/style/expression/collator.hpp>
#include <mbgl/util/platform.hpp>
#include <libnu/strcoll.h>
#include <libnu/unaccent.h>

#include <cstring>
#include <sstream>

/*
    The default implementation of Collator ignores locale.
    Case sensitivity and collation order are based on
    Default Unicode Collation Element Table (DUCET).

    Diacritic-insensitivity is implemented with nunicode's
    non-standard "unaccent" functionality, which is tailored
    to European languages.

    It would be possible to implement locale awareness using ICU,
    but would require bundling locale data.
*/

namespace {
std::string unaccent(const std::string& str)
{
    std::stringstream output;
    char const *itr = str.c_str(), *nitr;
    char const *end = itr + str.length();
    char lo[5] = { 0 };

    for (; itr < end; itr = nitr)
    {
        uint32_t code_point = 0;
        char const* buf = nullptr;

        nitr = _nu_tounaccent(itr, end, nu_utf8_read, &code_point, &buf, nullptr);
        if (buf != nullptr)
        {
            do
            {
                buf = NU_CASEMAP_DECODING_FUNCTION(buf, &code_point);
                if (code_point == 0) break;
                output.write(lo, nu_utf8_write(code_point, lo) - lo);
            }
            while (code_point != 0);
        }
        else
        {
            output.write(itr, nitr - itr);
        }
    }

    return output.str();
}
} // namespace

namespace mbgl {
namespace style {
namespace expression {

class Collator::Impl {
public:
    Impl(bool caseSensitive_, bool diacriticSensitive_, optional<std::string>)
        : caseSensitive(caseSensitive_)
        , diacriticSensitive(diacriticSensitive_)
    {}

    bool operator==(const Impl& other) const {
        return caseSensitive == other.caseSensitive &&
            diacriticSensitive == other.diacriticSensitive;
    }

    int compare(const std::string& lhs, const std::string& rhs) const {
        if (caseSensitive && diacriticSensitive) {
            return nu_strcoll(lhs.c_str(), rhs.c_str(),
                              nu_utf8_read, nu_utf8_read);
        } else if (!caseSensitive && diacriticSensitive) {
            return nu_strcasecoll(lhs.c_str(), rhs.c_str(),
                                  nu_utf8_read, nu_utf8_read);
        } else if (caseSensitive && !diacriticSensitive) {
            return nu_strcoll(unaccent(lhs).c_str(), unaccent(rhs).c_str(),
                              nu_utf8_read, nu_utf8_read);
        } else {
            return nu_strcasecoll(unaccent(lhs).c_str(), unaccent(rhs).c_str(),
                                  nu_utf8_read, nu_utf8_read);
        }
    }

    std::string resolvedLocale() const {
        return "";
    }
private:
    bool caseSensitive;
    bool diacriticSensitive;
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
