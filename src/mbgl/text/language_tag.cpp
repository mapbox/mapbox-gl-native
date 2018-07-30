#include <mbgl/text/language_tag.hpp>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wshadow"
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wshorten-64-to-32"
#pragma clang diagnostic ignored "-Wtautological-constant-compare"
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#pragma clang diagnostic pop
#pragma GCC diagnostic pop

#include <sstream>

/*
 ABNF for BCP 47 from: https://tools.ietf.org/html/bcp47

 Language-Tag  = langtag             ; normal language tags
               / privateuse          ; private use tag
               / grandfathered       ; grandfathered tags  NOT IMPLEMENTED

 langtag       = language
                 ["-" script]
                 ["-" region]
                 *("-" variant)
                 *("-" extension)
                 ["-" privateuse]

 language      = 2*3ALPHA            ; shortest ISO 639 code
                 ["-" extlang]       ; sometimes followed by
                                     ; extended language subtags
               / 4ALPHA              ; or reserved for future use
               / 5*8ALPHA            ; or registered language subtag

 extlang       = 3ALPHA              ; selected ISO 639 codes
                 *2("-" 3ALPHA)      ; permanently reserved

 script        = 4ALPHA              ; ISO 15924 code

 region        = 2ALPHA              ; ISO 3166-1 code
               / 3DIGIT              ; UN M.49 code

 variant       = 5*8alphanum         ; registered variants
               / (DIGIT 3alphanum)

 extension     = singleton 1*("-" (2*8alphanum))

 ; Single alphanumerics
 ; "x" reserved for private use
 singleton     = DIGIT               ; 0 - 9
               / %x41-57             ; A - W
               / %x59-5A             ; Y - Z
               / %x61-77             ; a - w
               / %x79-7A             ; y - z

 privateuse    = "x" 1*("-" (1*8alphanum))

 grandfathered = irregular           ; non-redundant tags registered
               / regular             ; during the RFC 3066 era

 irregular     = "en-GB-oed"         ; irregular tags do not match
                 / "i-ami"             ; the 'langtag' production and
                 / "i-bnn"             ; would not otherwise be
                 / "i-default"         ; considered 'well-formed'
                 / "i-enochian"        ; These tags are all valid,
                 / "i-hak"             ; but most are deprecated
                 / "i-klingon"         ; in favor of more modern
                 / "i-lux"             ; subtags or subtag
                 / "i-mingo"           ; combination
                 / "i-navajo"
                 / "i-pwn"
                 / "i-tao"
                 / "i-tay"
                 / "i-tsu"
                 / "sgn-BE-FR"
                 / "sgn-BE-NL"
                 / "sgn-CH-DE"

 regular       = "art-lojban"        ; these tags match the 'langtag'
                 / "cel-gaulish"       ; production, but their subtags
                 / "no-bok"            ; are not extended language
                 / "no-nyn"            ; or variant subtags: their meaning
                 / "zh-guoyu"          ; is defined by their registration
                 / "zh-hakka"          ; and all of these are deprecated
                 / "zh-min"            ; in favor of a more modern
                 / "zh-min-nan"        ; subtag or sequence of subtags
                 / "zh-xiang"

 alphanum      = (ALPHA / DIGIT)     ; letters and numbers

*/

namespace mbgl {

namespace qi = boost::spirit::qi;
namespace phoenix = boost::phoenix;
namespace ascii = boost::spirit::ascii;

template <typename Iterator>
struct bcp47_parser : qi::grammar<Iterator>
{
    bcp47_parser() : bcp47_parser::base_type(start)
    {
        using qi::lit;
        using qi::repeat;
        using qi::inf;
        using qi::eoi;
        using ascii::char_;
        using ascii::no_case;
        using ascii::digit;
        using ascii::alnum;
        using ascii::alpha;

        using boost::spirit::qi::_1;

        start %= no_case[langtag | privateuse | grandfathered];

        langtag %= (language) [phoenix::ref(languageTag.language) = _1]
            >> -("-" >> (script)[phoenix::ref(languageTag.script) = _1])
            >> -("-" >> (region)[phoenix::ref(languageTag.region) = _1])
            >> *("-" >> variant)
            >> *("-" >> extension)
            >> -("-" >> privateuse);

        language %= (repeat(2,3)[alpha] >> -("-" >> extlang))     // shortest ISO 639 code
                                                                  // sometimes followed by extended language subtags
            | repeat(4)[alpha]                                    // or reserved for future use
            | repeat(5,8)[alpha];                                 // or registered language subtag

        // We add lookaheads for "-"/eoi so that spurious matches on subtags don't prevent backtracking
        extlang = repeat(3)[alpha] >> (&lit('-') | eoi) >> repeat(0,2)["-" >> repeat(3)[alpha] >> (&lit('-') | eoi)];

        script = repeat(4)[alpha] >> (&lit('-') | eoi);

        region = (repeat(2)[alpha] | repeat(3)[digit]) >> (&lit('-') | eoi);

        variant = (repeat(5,8)[alnum] | (digit >> repeat(3,inf)[alnum])) >> (&lit('-') | eoi);

        extension = singleton >> +("-" >> repeat(2,8)[alnum]) >> (&lit('-') | eoi);

        singleton = digit | char_('a','w') | char_('y','z'); // "no-case" handles A-W and Y-Z

        privateuse = "x" >> +("-" >> repeat(1,8)[alnum]) >> (&lit('-') | eoi);

        grandfathered = regular | irregular;

        irregular = lit("en-GB-oed")
            | "i-ami"
            | "i-bnn"
            | "i-default"
            | "i-enochian"
            | "i-hak"
            | "i-klingon"
            | "i-lux"
            | "i-mingo"
            | "i-navajo"
            | "i-pwn"
            | "i-tao"
            | "i-tay"
            | "i-tsu"
            | "sgn-BE-FR"
            | "sgn-BE-NL"
            | "sgn-CH-DE";

        regular = lit("art-lojban")
            | "cel-gaulish"
            | "no-bok"
            | "no-nyn"
            | "zh-guoyu"
            | "zh-hakka"
            | "zh-min"
            | "zh-min-nan"
            | "zh-xiang";
    }

    qi::rule<Iterator> start;
    qi::rule<Iterator> langtag;
    qi::rule<Iterator, std::string()> language;
    qi::rule<Iterator> extlang;
    qi::rule<Iterator, std::string()> script;
    qi::rule<Iterator, std::string()> region;
    qi::rule<Iterator> variant;
    qi::rule<Iterator> extension;
    qi::rule<Iterator> singleton;
    qi::rule<Iterator> privateuse;
    qi::rule<Iterator> grandfathered;
    qi::rule<Iterator> irregular;
    qi::rule<Iterator> regular;

    LanguageTag languageTag;
};

LanguageTag LanguageTag::fromBCP47(const std::string& bcp47Tag) {
    typedef std::string::const_iterator iterator_type;
    typedef bcp47_parser<iterator_type> bcp47_parser;

    bcp47_parser parser;
    std::string::const_iterator iter = bcp47Tag.begin();
    std::string::const_iterator end = bcp47Tag.end();
    bool r = parse(iter, end, parser);
    if (r && iter == end) {
        return parser.languageTag;
    } else {
        // Invalid tags are treated as empty/"default"
        return LanguageTag();
    }
}

LanguageTag::LanguageTag(optional<std::string> language_, optional<std::string> script_, optional<std::string> region_)
    : language(std::move(language_))
    , script(std::move(script_))
    , region(std::move(region_))
{}

std::string LanguageTag::toBCP47() const {
    std::stringstream bcp47;
    if (!language) {
        // BCP 47 requires a language, but we're matching implementations that accept ""
        // to mean something like "default"
        return bcp47.str();
    } else {
        bcp47 << *language;
    }

    if (script) {
        bcp47 << "-" << *script;
    }

    if (region) {
        bcp47 << "-" << *region;
    }
    return bcp47.str();
}
} // end namespace mbgl
