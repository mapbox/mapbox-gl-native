#include <mbgl/util/http_header.hpp>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wshadow"
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#pragma GCC diagnostic pop

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

} // namespace http
} // namespace mbgl
