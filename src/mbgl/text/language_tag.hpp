#pragma once

#include <mbgl/util/optional.hpp>

#include <string>

/*
    Use LanguageTag to go back and forth between BCP 47 language tags
    and their component language/script/region.

    This implementation accepts but will not round-trip additional
    variant/extension/privateuse/grandfathered information in a BCP 47 tag.

    Why implement this?
        Mapbox Style Spec specifies locales with BCP 47
        Android and Intl.Collator implementations speak BCP 47 out of the box
        Darwin implementation requires translation to "Language Identifier"
        We're OK with not supporting extensions, but we want to succesfully
         parse any valid BCP 47 tag and get out the base language/script/region.

    Mozilla's version: https://dxr.mozilla.org/mozilla-central/source/intl/locale/MozLocale.cpp
        Looks like it actually supports a subset of BCP 47.
        See https://bugzilla.mozilla.org/show_bug.cgi?id=bcp47

    Chromium is based on ICU version: https://ssl.icu-project.org/apiref/icu4c/uloc_8h.html
        Getting all the locale information is overkill for us, we just want
         language/script/region.
 */

namespace mbgl {

struct LanguageTag {
    LanguageTag() = default;
    LanguageTag(optional<std::string> language_, optional<std::string> script_, optional<std::string> region_);

    static LanguageTag fromBCP47(const std::string& bcp47Tag);
    std::string toBCP47() const;

    optional<std::string> language; // ISO 639
    optional<std::string> script;   // ISO 15924
    optional<std::string> region;   // ISO 3316-1 || UN M.49
};

} // end namespace mbgl
