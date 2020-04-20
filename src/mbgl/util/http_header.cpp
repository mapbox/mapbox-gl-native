#include <mbgl/util/http_header.hpp>

#include <mbgl/util/chrono.hpp>
#include <mbgl/util/string.hpp>

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>

namespace mbgl {
namespace http {

CacheControl CacheControl::parse(const std::string& value) {
    namespace qi = boost::spirit::qi;
    namespace phoenix = boost::phoenix;

    CacheControl result;
    qi::phrase_parse(value.begin(), value.end(), (
        (qi::lit("must-revalidate") [ phoenix::ref(result.mustRevalidate) = true ]) |
        (qi::lit("max-age") >> '=' >> qi::ulong_long [ phoenix::ref(result.maxAge) = qi::_1 ]) |
        (*(('"' >> *(('\\' >> qi::char_) | (qi::char_ - '"')) >> '"') | (qi::char_ - '"' - ',')))
    ) % ',', qi::ascii::space);
    return result;
}

optional<Timestamp> CacheControl::toTimePoint() const {
    return maxAge ? util::now() + Seconds(*maxAge) : optional<Timestamp>{};
}

optional<Timestamp> parseRetryHeaders(const optional<std::string>& retryAfter,
                                      const optional<std::string>& xRateLimitReset) {
    if (retryAfter) {
        try {
            auto secs = std::chrono::seconds(std::stoi(*retryAfter));
            return std::chrono::time_point_cast<Seconds>(util::now() + secs);
        } catch (...) {
            return util::parseTimestamp((*retryAfter).c_str());
        }
    } else if (xRateLimitReset) {
        try {
            return util::parseTimestamp(std::stoi(*xRateLimitReset));
        } catch (...) {
            return {};
        }
    }

    return {};
}

} // namespace http
} // namespace mbgl
